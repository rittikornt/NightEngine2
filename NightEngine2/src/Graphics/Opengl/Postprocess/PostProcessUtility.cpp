/*!
  @file PostProcessUtility.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Bloom
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
      m_target1Texture = Texture::GenerateNullTexture(m_resolution.x, m_resolution.y
        , Texture::Channel::RGBA16F, Texture::Channel::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

      m_temp1Fbo.Init();
      m_temp1Fbo.AttachTexture(m_target1Texture);
      m_temp1Fbo.Bind();
      m_temp1Fbo.Unbind();

      //FBO Target 2
      m_target2Texture = Texture::GenerateNullTexture(m_resolution.x, m_resolution.y
        , Texture::Channel::RGBA16F, Texture::Channel::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

      m_temp2Fbo.Init();
      m_temp2Fbo.AttachTexture(m_target2Texture);
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
      if (useKawase)
      {
        const int k_maxBlurIteration = 4;
        iteration = std::min(iteration, k_maxBlurIteration);
        for (int i = 0; i < iteration; ++i)
        {
          FrameBufferObject& fbo = i % 2 == 0 ? m_temp1Fbo : m_temp2Fbo;
          Texture& tex = i % 2 == 0 ? m_target2Texture : m_target1Texture;
          fbo.Bind();
          {
            m_kawaseBlurShader.Bind();
            {
              m_kawaseBlurShader.SetUniform("u_iteration", i);

              if (i == 0)
              {
                target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
                glViewport(0, 0, resolution.x, resolution.y);
              }
              else
              {
                tex.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
                glViewport(0, 0, m_resolution.x, m_resolution.y);
              }

              //Render
              screenVAO.Draw();
            }
            m_kawaseBlurShader.Unbind();
          }
          fbo.Unbind();
        }
      }
      else
      {
        glm::vec2 blurDir = glm::vec2(0, 0);
        for (int i = 0; i < iteration; ++i)
        {
          FrameBufferObject& fbo = i % 2 == 0 ? m_temp1Fbo : m_temp2Fbo;
          Texture& tex = i % 2 == 0 ? m_target2Texture : m_target1Texture;
          fbo.Bind();
          {
            m_blurShader.Bind();
            {
              blurDir = glm::vec2((i + 1) % 2, i % 2);
              m_blurShader.SetUniform("u_dir", blurDir);

              if (i == 0)
              {
                target.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
                glViewport(0, 0, resolution.x, resolution.y);
              }
              else
              {
                tex.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
                glViewport(0, 0, m_resolution.x, m_resolution.y);
              }

              //Render
              screenVAO.Draw();
            }
            m_blurShader.Unbind();
          }
          fbo.Unbind();
        }
      }

      //Copy back to target
      FrameBufferObject& fbo = (iteration - 1) % 2 == 0 ? m_temp1Fbo : m_temp2Fbo;
      fbo.CopyToTexture(target
        , resolution.x, resolution.y);
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