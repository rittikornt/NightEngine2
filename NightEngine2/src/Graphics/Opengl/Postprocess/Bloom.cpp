/*!
  @file Bloom.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Bloom
*/
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"

namespace Rendering
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(Bloom)

    //TODO: Better Bloom Falloff
    void Bloom::Init(int width, int height)
    {
      INIT_POSTPROCESSEFFECT();
      m_resolution.x = width, m_resolution.y = height;

      //Buffers for 5 down scaled version
      glm::ivec2 renderSize{ m_resolution };
      for (int i = 0; i < k_bloomPyramidCount; ++i)
      {
        m_bloomTexture[i] = Texture::GenerateNullTexture(renderSize.x, renderSize.y
          , Texture::Channel::RGBA16F, Texture::Channel::RGBA
          , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

        renderSize /= 2;
      }

      //FBO Target
      m_targetTexture = Texture::GenerateNullTexture(m_resolution.x, m_resolution.y
        , Texture::Channel::RGBA16F, Texture::Channel::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

      m_bloomFbo.Init();
      m_bloomFbo.AttachTexture(m_targetTexture);
      m_bloomFbo.Bind();
      m_bloomFbo.Unbind();

      //Shaders
      m_thresholdShader.Create();
      m_thresholdShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_thresholdShader.AttachShaderFile("Postprocess/brightness_threshold.frag");
      m_thresholdShader.Link();

      m_blitCopyShader.Create();
      m_blitCopyShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_blitCopyShader.AttachShaderFile("Utility/blitCopy.frag");
      m_blitCopyShader.Link();
      
      m_blurShader.Create();
      m_blurShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_blurShader.AttachShaderFile("Postprocess/gaussian_blur.frag");
      m_blurShader.Link();

      m_kawaseBlurShader.Create();
      m_kawaseBlurShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_kawaseBlurShader.AttachShaderFile("Postprocess/kawase_blur.frag");
      m_kawaseBlurShader.Link();

      m_bloomShader.Create();
      m_bloomShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_bloomShader.AttachShaderFile("Postprocess/bloom.frag");
      m_bloomShader.Link();

      //Set Uniform
      RefreshTextureUniforms();

      m_blurIteration = 4;
      m_bloomThreshold = 6.0f;
    }

    void Bloom::Apply(VertexArrayObject& screenVAO
      , Texture& screenTexture)
    {
      //Render 5 versions of downsample threshold color
      glDisable(GL_DEPTH_TEST);
      glm::ivec2 renderSize{ m_resolution };
      {
        //Adjust resolution scale
        glViewport(0, 0, renderSize.x, renderSize.y);

        //Threshold Shader
        m_bloomFbo.Bind();
        {
          glClear(GL_COLOR_BUFFER_BIT);

          //Get the brightness threshold
          m_thresholdShader.Bind();
          {
            m_thresholdShader.SetUniform("u_threshold", m_bloomThreshold);
            screenTexture.BindToTextureUnit(0);

            screenVAO.Draw();
          }
          m_thresholdShader.Unbind();
        }
        m_bloomFbo.Unbind();

        //Copy and scale down the size
        m_bloomFbo.CopyToTexture(m_bloomTexture[0]
          , renderSize.x, renderSize.y);
        renderSize /= 2;
      }

      //BlitCopy Down Scaling
      for (int i = 1; i < k_bloomPyramidCount; ++i)
      {
        //Adjust resolution scale
        glViewport(0, 0, renderSize.x, renderSize.y);

        //Threshold Shader
        m_bloomFbo.Bind();
        {
          glClear(GL_COLOR_BUFFER_BIT);

          //DownScaling Shader
          m_blitCopyShader.Bind();
          {
            m_bloomTexture[i - 1].BindToTextureUnit(0);
            screenVAO.Draw();
          }
          m_blitCopyShader.Unbind();
        }
        m_bloomFbo.Unbind();

        //Copy and scale down the size
        m_bloomFbo.CopyToTexture(m_bloomTexture[i]
          , renderSize.x, renderSize.y);
        renderSize /= 2;
      }

      //Blur all texture
      //Not blurring the Mip0 since its too expensive
      //BlurTarget(m_bloomTexture[0], screenVAO
      //  , m_resolution, m_blurIteration, m_useKawaseBlur);

      BlurTarget(m_bloomTexture[1], screenVAO
        , m_resolution / 2, m_blurIteration, m_useKawaseBlur);
      BlurTarget(m_bloomTexture[2], screenVAO
        , m_resolution / 4, m_blurIteration, m_useKawaseBlur);
      BlurTarget(m_bloomTexture[3], screenVAO
        , m_resolution / 8, m_blurIteration, m_useKawaseBlur);
      BlurTarget(m_bloomTexture[4], screenVAO
        , m_resolution / 16, m_blurIteration, m_useKawaseBlur);

      //Combine all blurred texture
      glViewport(0, 0, m_resolution.x, m_resolution.y);
      m_bloomFbo.Bind();
      {
        glClear(GL_COLOR_BUFFER_BIT);
        m_bloomShader.Bind();
        {
          m_bloomShader.SetUniform("u_intensity", m_intensity);
          //Bind all texture
          for (int i = 0; i < k_bloomPyramidCount; ++i)
          {
            m_bloomTexture[i].BindToTextureUnit(i);
          }

          //Render
          screenVAO.Draw();
        }
        m_bloomShader.Unbind();
      }
      m_bloomFbo.Unbind();
    }

    void Bloom::BlurTarget(Texture& target
      , VertexArrayObject& screenVAO
      , glm::ivec2 resolution, int iteration, bool useKawase)
    {
      //Render Resolution and clear color
      m_bloomFbo.Bind();
      {
        glViewport(0, 0, resolution.x, resolution.y);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_bloomFbo.Unbind();

      //Blur the texture
      if (useKawase)
      {
        const int k_maxBlurIteration = 4;
        iteration = std::min(iteration, k_maxBlurIteration);
        for (int i = 0; i < iteration; ++i)
        {
          m_bloomFbo.Bind();
          {
            m_kawaseBlurShader.Bind();
            {
              m_kawaseBlurShader.SetUniform("u_iteration", i);

              target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);

              //Render
              screenVAO.Draw();
            }
            m_kawaseBlurShader.Unbind();
          }
          m_bloomFbo.Unbind();

          //Copy back to target
          m_bloomFbo.CopyToTexture(target
            , resolution.x, resolution.y);
        }
      }
      else
      {
        for (int i = 0; i < m_blurIteration; ++i)
        {
          m_bloomFbo.Bind();
          {
            m_blurShader.Bind();
            {
              m_blurDir = glm::vec2((i + 1) % 2, i % 2);
              m_blurShader.SetUniform("u_dir", m_blurDir);

              target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);

              //Render
              screenVAO.Draw();
            }
            m_blurShader.Unbind();
          }
          m_bloomFbo.Unbind();

          //Copy back to target
          m_bloomFbo.CopyToTexture(target
            , resolution.x, resolution.y);
        }
      }
    }

    void Bloom::Clear(void)
    {
      m_bloomFbo.Bind();
      {
        //Clear with black color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_bloomFbo.Unbind();
    }

    void Bloom::RefreshTextureUniforms(void)
    {
      m_thresholdShader.Bind();
      {
        m_thresholdShader.SetUniform("u_screenTexture", 0);
      }
      m_thresholdShader.Unbind();

      m_blitCopyShader.Bind();
      {
        m_blitCopyShader.SetUniform("u_screenTexture", 0);
      }
      m_blitCopyShader.Unbind();

      m_blurShader.Bind();
      {
        m_blurShader.SetUniform("u_screenTexture", 0);
      }
      m_blurShader.Unbind();

      m_bloomShader.Bind();
      {
        m_bloomShader.SetUniform("u_screenTexture[0]", 0);
        m_bloomShader.SetUniform("u_screenTexture[1]", 1);
        m_bloomShader.SetUniform("u_screenTexture[2]", 2);
        m_bloomShader.SetUniform("u_screenTexture[3]", 3);
        m_bloomShader.SetUniform("u_screenTexture[4]", 4);
      }
      m_bloomShader.Unbind();
    }
  }
}