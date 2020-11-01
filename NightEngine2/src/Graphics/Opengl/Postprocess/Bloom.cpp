/*!
  @file Bloom.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Bloom
*/
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"

#include "Graphics/Opengl/Postprocess/PostProcessUtility.hpp"

namespace NightEngine::Rendering::Opengl
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(Bloom)

    static glm::ivec2 g_bloomResolutions[k_bloomPyramidCount + 1];

    //TODO: Better Bloom Falloff
    void Bloom::LazyInit(int width, int height)
    {
      if (m_resolution.x == 0)
      {
        INIT_POSTPROCESSEFFECT();
        m_resolution.x = width, m_resolution.y = height;

        //FBO for 5 down scaled version
        glm::ivec2 renderSize{ m_resolution };
        std::string name = "";
        for (int i = 0; i < k_bloomPyramidCount + 1; ++i)
        {
          g_bloomResolutions[i] = renderSize;

          {
            name = "BloomDownScale (" + std::to_string(g_bloomResolutions[i].x)
              + "x" + std::to_string(g_bloomResolutions[i].y) + ")";
            m_bloomDownscaleTexture[i] = Texture::GenerateRenderTexture(renderSize.x, renderSize.y
              , Texture::Format::RGB16F, Texture::Format::RGBA
              , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
            m_bloomDownscaleTexture[i].SetName(name.c_str());

            m_bloomDownscaleFbo[i].Init();
            m_bloomDownscaleFbo[i].AttachColorTexture(m_bloomDownscaleTexture[i]);
            m_bloomDownscaleFbo[i].Bind();
            m_bloomDownscaleFbo[i].Unbind();
          }

          {
            name = "BloomUpScale (" + std::to_string(g_bloomResolutions[i].x)
              + "x" + std::to_string(g_bloomResolutions[i].y) + ")";
            m_bloomUpscaleTexture[i] = Texture::GenerateRenderTexture(renderSize.x, renderSize.y
              , Texture::Format::RGB16F, Texture::Format::RGBA
              , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
            m_bloomUpscaleTexture[i].SetName(name.c_str());

            m_bloomUpscaleFbo[i].Init();
            m_bloomUpscaleFbo[i].AttachColorTexture(m_bloomUpscaleTexture[i]);
            m_bloomUpscaleFbo[i].Bind();
            m_bloomUpscaleFbo[i].Unbind();
          }

          renderSize /= 2;
        }

        //FBO Target
        {
          m_targetTexture = Texture::GenerateRenderTexture(m_resolution.x, m_resolution.y
            , Texture::Format::RGBA16F, Texture::Format::RGBA
            , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
          m_targetTexture.SetName("BloomResult");

          m_targetFbo.Init();
          m_targetFbo.AttachColorTexture(m_targetTexture);
          m_targetFbo.Bind();
          m_targetFbo.Unbind();
        }

        //Shaders
        m_thresholdShader.Create();
        m_thresholdShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
        m_thresholdShader.AttachShaderFile("Postprocess/brightness_threshold.frag");
        m_thresholdShader.Link();

        m_upscalingShader.Create();
        m_upscalingShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
        m_upscalingShader.AttachShaderFile("Postprocess/bloom_upscale.frag");
        m_upscalingShader.Link();

        m_blitCopyShader.Create();
        m_blitCopyShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
        m_blitCopyShader.AttachShaderFile("Utility/blitCopy.frag");
        m_blitCopyShader.Link();

        m_bloomShader.Create();
        m_bloomShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
        m_bloomShader.AttachShaderFile("Postprocess/bloom.frag");
        m_bloomShader.Link();

        //Set Uniform
        RefreshTextureUniforms();
      }
      else
      {
        if (m_resolution.x != width || m_resolution.y != height)
        {
          m_resolution.x = width, m_resolution.y = height;

          glm::ivec2 renderSize{ m_resolution };
          for (int i = 0; i < k_bloomPyramidCount + 1; ++i)
          {
            g_bloomResolutions[i] = renderSize;
            m_bloomDownscaleTexture[i].Resize(renderSize.x, renderSize.y, Texture::PixelFormat::RGBA);
            m_bloomUpscaleTexture[i].Resize(renderSize.x, renderSize.y, Texture::PixelFormat::RGBA);
            renderSize /= 2;
          }
          m_targetTexture.Resize(width, height, Texture::PixelFormat::RGBA);
        }
      }
    }

    void Bloom::Apply(VertexArrayObject& screenVAO
      , Texture& screenTexture, PostProcessUtility& ppUtility)
    {
      //Render 5 versions of downsample threshold color
      glDisable(GL_DEPTH_TEST);

      int offset = m_halfResolution ? 1 : 0;
      int firstIndex = 0 + offset;
      int lastIndex = k_bloomPyramidCount - 1 + offset;

      //Render the Threshold screen texture
      {
        int i = firstIndex;
        //Adjust resolution scale
        glViewport(0, 0, g_bloomResolutions[i].x, g_bloomResolutions[i].y);

        //Threshold Shader for Mip0
        m_bloomDownscaleFbo[i].Bind();
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
        m_bloomDownscaleFbo[i].Unbind();
      }

      //BlitCopy Down Scaling
      for (int i = firstIndex + 1; i < lastIndex + 1; ++i)
      {
        //Adjust resolution scale
        glViewport(0, 0, g_bloomResolutions[i].x, g_bloomResolutions[i].y);

        //DownScaling Shader
        m_bloomDownscaleFbo[i].Bind();
        {
          glClear(GL_COLOR_BUFFER_BIT);

          m_blitCopyShader.Bind();
          {
            m_bloomDownscaleTexture[i - 1].BindToTextureUnit(0);
            screenVAO.Draw();
          }
          m_blitCopyShader.Unbind();
        }
        m_bloomDownscaleFbo[i].Unbind();
      }

      //Blur all texture
      glm::vec4 clearColor = glm::vec4{ 0.0f,0.0f,0.0f,1.0f };
      for (int i = lastIndex; i >= firstIndex; --i)
      {
        ppUtility.BlurTarget(clearColor, m_bloomDownscaleFbo[i], m_bloomDownscaleTexture[i], screenVAO
          , g_bloomResolutions[i], m_blurIteration, m_useKawaseBlur);
      }

      //Up Scaling Passes
      if (m_upscalingPass)
      {
        //Clear all the upscale texture
        for (int i = 0; i < k_bloomPyramidCount; ++i)
        {
          m_bloomUpscaleFbo[i].Bind();
          {
            glClear(GL_COLOR_BUFFER_BIT);
          }
          m_bloomUpscaleFbo[i].Unbind();
        }

        for (int i = lastIndex - 1; i >= firstIndex; --i)
        {
          //Adjust resolution scale
          glViewport(0, 0, g_bloomResolutions[i].x, g_bloomResolutions[i].y);

          m_bloomUpscaleFbo[i].Bind();
          {
            m_upscalingShader.Bind();
            {
              //Low mip
              if (i == lastIndex - 1)
              {
                m_bloomDownscaleTexture[i + 1].BindToTextureUnit(0);
              }
              else
              {
                m_bloomUpscaleTexture[i + 1].BindToTextureUnit(0);
              }
              //High mip
              m_bloomDownscaleTexture[i].BindToTextureUnit(1);

              m_upscalingShader.SetUniform("u_scattering"
                , std::clamp(m_scattering, 0.0f, 1.0f));
              screenVAO.Draw();
            }
            m_upscalingShader.Unbind();
          }
          m_bloomUpscaleFbo[i].Unbind();
        }
      }

      //Combine all blurred texture
      glViewport(0, 0, m_resolution.x, m_resolution.y);
      m_targetFbo.Bind();
      {
        glClear(GL_COLOR_BUFFER_BIT);
        m_bloomShader.Bind();
        {
          m_bloomShader.SetUniform("u_intensity", m_intensity);
          m_bloomShader.SetUniform("u_scattering"
            , std::clamp(m_scattering, 0.0f, 1.0f));
          //Bind all texture
          for (int i = 0; i < k_bloomPyramidCount; ++i)
          {
            if (m_upscalingPass)
            {
              m_bloomUpscaleTexture[i].BindToTextureUnit(i);
            }
            else
            {
              m_bloomDownscaleTexture[i + offset].BindToTextureUnit(i);
            }
          }
      
          //Render
          screenVAO.Draw();
        }
        m_bloomShader.Unbind();
      }
      m_targetFbo.Unbind();
    }

    void Bloom::Clear(void)
    {
      m_targetFbo.Bind();
      {
        //Clear with black color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_targetFbo.Unbind();
    }

    void Bloom::RefreshTextureUniforms(void)
    {
      m_thresholdShader.Bind();
      {
        m_thresholdShader.SetUniform("u_screenTexture", 0);
      }
      m_thresholdShader.Unbind();

      m_upscalingShader.Bind();
      {
        m_upscalingShader.SetUniform("u_lowMipTexture", 0);
        m_upscalingShader.SetUniform("u_highMipTexture", 1);
      }
      m_upscalingShader.Unbind();

      m_blitCopyShader.Bind();
      {
        m_blitCopyShader.SetUniform("u_screenTexture", 0);
      }
      m_blitCopyShader.Unbind();

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