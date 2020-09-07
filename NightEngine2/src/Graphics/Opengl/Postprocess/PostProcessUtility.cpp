/*!
  @file PostProcessUtility.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of PostProcessUtility
*/
#include "Graphics/Opengl/Postprocess/PostProcessUtility.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"

namespace Rendering
{
  namespace Postprocess
  {
    void PostProcessUtility::Init(int width, int height)
    {
      m_resolution.x = width, m_resolution.y = height;

      //FBO Target 1
      m_target1Texture = Texture::GenerateRenderTexture(m_resolution.x, m_resolution.y
        , Texture::Format::RGBA16F, Texture::Format::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

      m_temp1Fbo.Init();
      m_temp1Fbo.AttachColorTexture(m_target1Texture);
      m_temp1Fbo.Bind();
      m_temp1Fbo.Unbind();

      //FBO Target 2
      m_target2Texture = Texture::GenerateRenderTexture(m_resolution.x, m_resolution.y
        , Texture::Format::RGBA16F, Texture::Format::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

      m_temp2Fbo.Init();
      m_temp2Fbo.AttachColorTexture(m_target2Texture);
      m_temp2Fbo.Bind();
      m_temp2Fbo.Unbind();

      //Shaders
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

      //Set Uniform
      RefreshTextureUniforms();
    }

    void PostProcessUtility::BlurTarget(glm::vec4 clearColor, Texture& target
      , VertexArrayObject& screenVAO, glm::ivec2 resolution, int iteration, bool useKawase)
    {
      //Render Resolution and clear color
      glViewport(0, 0, m_resolution.x, m_resolution.y);
      m_clearColor = clearColor;
      Clear();

      //Blur the texture
      Shader& blurShader = useKawase? m_kawaseBlurShader: m_blurShader;
      if (useKawase)
      {
        const int k_maxBlurIteration = 4;
        iteration = std::min(iteration, k_maxBlurIteration);
      }

      glm::vec2 blurDir = glm::vec2(0, 0);
      glm::vec2 uvScale = glm::vec2((float)resolution.x/ (float)m_resolution.x
        , (float)resolution.y/ (float)m_resolution.y);

      glViewport(0, 0, resolution.x, resolution.y);
      for (int i = 0; i < iteration; ++i)
      {
        FrameBufferObject& fbo = i % 2 == 0 ? m_temp1Fbo : m_temp2Fbo;
        Texture& tex = i % 2 == 0 ? m_target2Texture : m_target1Texture;
        fbo.Bind();
        {
          blurShader.Bind();
          {
            //Set Uniforms for different blur shader
            if (useKawase)
            {
              blurShader.SetUniform("u_iteration", i);
            }
            else
            {
              blurDir = glm::vec2((i + 1) % 2, i % 2);
              blurShader.SetUniform("u_dir", blurDir);
            }

            if (i == 0)
            {
              //In first pass, fits the variable sized texture into screen resolution texture
              target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
              blurShader.SetUniform("u_uvScale", glm::vec2(1.0f, 1.0f));

            }
            else
            {
              //Blurring the whole texture at the target resolution
              //Must modify sampling UV to match the portion of the screen resolution texture you want to sample
              tex.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
              blurShader.SetUniform("u_uvScale", uvScale);
            }

            //Render
            screenVAO.Draw();
          }
          blurShader.Unbind();
        }
        fbo.Unbind();
      }

      //Copy back to target
      FrameBufferObject& fbo = (iteration - 1) % 2 == 0 ? m_temp1Fbo : m_temp2Fbo;
      fbo.CopyToTexture(target
        , resolution.x, resolution.y);
    }

    void PostProcessUtility::BlurTarget(glm::vec4 clearColor, FrameBufferObject& targetFbo, Texture& target, VertexArrayObject& screenVAO, glm::ivec2 resolution, int iteration, bool useKawase)
    {
      //Render Resolution and clear color
      glViewport(0, 0, m_resolution.x, m_resolution.y);
      m_clearColor = clearColor;
      Clear();

      //Blur the texture
      Shader& blurShader = useKawase ? m_kawaseBlurShader : m_blurShader;
      if (useKawase)
      {
        const int k_maxBlurIteration = 4;
        iteration = std::min(iteration, k_maxBlurIteration);
      }

      glm::vec2 blurDir = glm::vec2(0, 0);
      glm::vec2 uvScale = glm::vec2((float)resolution.x / (float)m_resolution.x
        , (float)resolution.y / (float)m_resolution.y);

      glViewport(0, 0, resolution.x, resolution.y);
      for (int i = 0; i < iteration; ++i)
      {
        //Draw to the target FBO for last iteration, or else just pingponging
        FrameBufferObject& fbo = (i == iteration-1) ? targetFbo :
          (i % 2 == 0 ? m_temp1Fbo : m_temp2Fbo);
        Texture& tex = i % 2 == 0 ? m_target2Texture : m_target1Texture;
        fbo.Bind();
        {
          blurShader.Bind();
          {
            //Set Uniforms for different blur shader
            if (useKawase)
            {
              blurShader.SetUniform("u_iteration", i);
            }
            else
            {
              blurDir = glm::vec2((i + 1) % 2, i % 2);
              blurShader.SetUniform("u_dir", blurDir);
            }

            if (i == 0)
            {
              //In first pass, fits the variable sized texture into screen resolution texture
              target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
              blurShader.SetUniform("u_uvScale", glm::vec2(1.0f, 1.0f));
            }
            else
            {
              //Blurring the whole texture at the target resolution
              //Must modify sampling UV to match the portion of the screen resolution texture you want to sample
              tex.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
              blurShader.SetUniform("u_uvScale", uvScale);
            }

            //Render
            screenVAO.Draw();
          }
          blurShader.Unbind();
        }
        fbo.Unbind();
      }
    }

    void PostProcessUtility::Clear(void)
    {
      m_temp1Fbo.Bind();
      {
        glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_temp1Fbo.Unbind();

      m_temp2Fbo.Bind();
      {
        glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_temp2Fbo.Unbind();
    }

    void PostProcessUtility::RefreshTextureUniforms(void)
    {
      m_blurShader.Bind();
      {
        m_blurShader.SetUniform("u_screenTexture", 0);
      }
      m_blurShader.Unbind();

      m_kawaseBlurShader.Bind();
      {
        m_kawaseBlurShader.SetUniform("u_screenTexture", 0);
      }
      m_kawaseBlurShader.Unbind();
    }
  }
}