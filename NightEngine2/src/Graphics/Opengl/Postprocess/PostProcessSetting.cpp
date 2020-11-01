/*!
  @file PostProcessSetting.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of PostProcessSetting
*/
#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"

#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/RenderPass/GBuffer.hpp"

#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"
#include "Graphics/Opengl/DebugMarker.hpp"
#include "Graphics/Opengl/Material.hpp"

namespace NightEngine::Rendering::Opengl
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(PostProcessSetting)

    void PostProcessSetting::LazyInit(CameraObject& camera, GBuffer& gbuffer)
    {
      if (m_resolution.x == 0)
      {
        //Init Resources
        m_uberPostMaterial.InitShader("Utility/fullscreenTriangle.vert"
          , "Postprocess/uberpost.frag");

        m_blitCopyMaterial.InitShader("Utility/fullscreenTriangle.vert"
          , "Utility/blitCopy.frag");

        m_ssaoComposite.InitShader("Utility/fullscreenTriangle.vert"
          , "Postprocess/ssao_composite.frag");

        //FXAA
        m_fxaaPP.Init(m_resolution.x, m_resolution.y);

        m_postProcessEffects.emplace_back(&m_bloomPP);
        m_postProcessEffects.emplace_back(&m_ssaoPP);
        m_postProcessEffects.emplace_back(&m_fxaaPP);
        m_postProcessEffects.emplace_back(&m_taaPP);
      }

      {
        m_resolution.x = camera.m_scaledPixelResolution.x;
        m_resolution.y = camera.m_scaledPixelResolution.y;

        //Postfx
        m_ppUtility.LazyInit(m_resolution.x, m_resolution.y);

        //Bloom
        m_bloomPP.LazyInit(m_resolution.x, m_resolution.y);

        //SSAO
        m_ssaoPP.LazyInit(m_resolution.x, m_resolution.y, gbuffer);

        //TAA
        m_taaPP.LazyInit(m_resolution.x, m_resolution.y);
      }
    }

    void PostProcessSetting::Apply(const PostProcessContext& context)
    {
      CameraObject* camera = context.camera;
      bool additionalCompositePass = camera->m_renderScale != 1.0f;
      glm::ivec2 screenSize = camera->GetScreenSize();
      float screenZoomScale = context.screenZoomScale;

      GBuffer* gbuffer = context.gbuffer;
      FrameBufferObject* sceneFBO = context.sceneFBO;
      VertexArrayObject* screenVAO = context.screenVAO;
      Texture* screenTexture = context.screenTexture;

      //SSAO
      bool shouldCompositeSSAOToSceneRT = false;// m_taaPP.m_enable&& m_taaPP.m_beforeTonemapping;
      if (m_ssaoPP.m_enable)
      {
        DebugMarker::PushDebugGroup("SSAO Pass");
        {
          m_ssaoPP.Apply(*screenVAO, *camera
            , *gbuffer, m_ppUtility);

          //Turn off for now as this will overwrite the emissive
          if (shouldCompositeSSAOToSceneRT)
          {
            //Combine ssao to scene texture
            sceneFBO->Bind();
            {
              m_ssaoComposite.Bind(false);
              {
                Shader& shader = m_ssaoComposite.GetShader();
                shader.SetUniform("u_screenTexture", 0);
                shader.SetUniform("u_ssaoTexture", 1);

                //PP Texture
                {
                  screenTexture->BindToTextureUnit(0);
                  m_ssaoPP.m_ssaoTexture.BindToTextureUnit(1);
                }

                screenVAO->Draw();
              }
              m_ssaoComposite.Unbind();
            }
            sceneFBO->Unbind();
          }
        }
        DebugMarker::PopDebugGroup();
      }
      else
      {
        m_ssaoPP.Clear();
      }

      //TAA before tonemapping
      if (m_taaPP.m_enable && m_taaPP.m_beforeTonemapping)
      {
        DebugMarker::PushDebugGroup("TAA");
        {
          m_taaPP.Apply(*screenVAO
            , *gbuffer, *screenTexture
            , *sceneFBO, *camera);
        }
        DebugMarker::PopDebugGroup();
      }

      //Bloom
      if (m_bloomPP.m_enable)
      {
        DebugMarker::PushDebugGroup("Bloom Pass");
        {
          m_bloomPP.Apply(*screenVAO
            , m_taaPP.m_enable && m_taaPP.m_beforeTonemapping? 
            m_taaPP.m_currRT: *(context.screenTexture)
            , m_ppUtility);
        }
        DebugMarker::PopDebugGroup();
      }
      else
      {
        m_bloomPP.Clear();
      }

      //*************************************************
      // Uber pass
      //*************************************************
      DebugMarker::PushDebugGroup("UberPostProcess");
      {
        glViewport(0, 0, screenSize.x, screenSize.y);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        //Draw Screen
        auto time = context.time;
        sceneFBO->Bind();
        {
          m_uberPostMaterial.Bind(false);
          {
            Shader& shader = m_uberPostMaterial.GetShader();
            shader.SetUniform("u_screenTexture", 0);
            shader.SetUniform("u_bloomTexture", 1);
            shader.SetUniform("u_ssaoTexture", 2);
            shader.SetUniform("u_exposure", 1.0f);
            shader.SetUniform("u_time", time);
            shader.SetUniform("u_useSSAO", !shouldCompositeSSAOToSceneRT);

            //PP Texture
            {
              if (m_taaPP.m_enable && m_taaPP.m_beforeTonemapping)
              {
                m_taaPP.m_currRT.BindToTextureUnit(0);
              }
              else
              {
                screenTexture->BindToTextureUnit(0);
              }
              m_bloomPP.m_targetTexture.BindToTextureUnit(1);
              m_ssaoPP.m_ssaoTexture.BindToTextureUnit(2);
            }

            screenVAO->Draw();
          }
          m_uberPostMaterial.Unbind();
        }
        sceneFBO->Unbind();
      }
      DebugMarker::PopDebugGroup();

      //TAA after tonemapping
      if (m_taaPP.m_enable && !m_taaPP.m_beforeTonemapping)
      {
        DebugMarker::PushDebugGroup("TAA");
        {
          m_taaPP.Apply(*screenVAO
            , *gbuffer, *screenTexture
            , *sceneFBO, *camera);
        }
        DebugMarker::PopDebugGroup();
      }

      //*************************************************
      // Final Draw pass
      //*************************************************
      DebugMarker::PushDebugGroup("Final Draw");
      {
        //Required additional composite pass from abitrary RT resolution onto screen
        if (additionalCompositePass)
        {
          sceneFBO->Bind();
        }
        {
          //FXAA
          if (m_fxaaPP.m_enable)
          {
            DebugMarker::PushDebugGroup("FXAA");
            {
              m_fxaaPP.ApplyToScreen(*screenVAO
                , m_taaPP.m_enable && !m_taaPP.m_beforeTonemapping ?
                m_taaPP.m_currRT : *(context.screenTexture)
                , screenZoomScale);
            }
            DebugMarker::PopDebugGroup();
          }
          else
          {
            m_blitCopyMaterial.Bind(false);
            {
              m_blitCopyMaterial.GetShader().SetUniform("u_scale", screenZoomScale);

              m_blitCopyMaterial.GetShader().SetUniform("u_screenTexture", 0);

              //Use TAA result if its after tonemapping
              if (m_taaPP.m_enable && !m_taaPP.m_beforeTonemapping)
              {
                m_taaPP.m_currRT.BindToTextureUnit(0);
              }
              else
              {
                screenTexture->BindToTextureUnit(0);
              }

              //Draw Quad
              screenVAO->Draw();
            }
            m_blitCopyMaterial.Unbind();
          }
        }
        if (additionalCompositePass)
        {
          sceneFBO->Unbind();
          sceneFBO->CopyBufferToTarget(camera->m_scaledPixelResolution.x, camera->m_scaledPixelResolution.y
            , camera->m_windowPixelResolution.x, camera->m_windowPixelResolution.y
            , 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
      }
      DebugMarker::PopDebugGroup();
    }

    void PostProcessSetting::Clear(void)
    {
      m_ppUtility.Clear();

      m_ssaoPP.Clear();
      m_bloomPP.Clear();
    }

    void PostProcessSetting::RefreshTextureUniforms(void)
    {
      m_ppUtility.RefreshTextureUniforms();

      //Postprocessing
      m_bloomPP.RefreshTextureUniforms();
      m_ssaoPP.RefreshTextureUniforms();
      m_fxaaPP.RefreshTextureUniforms();
      m_taaPP.RefreshTextureUniforms();
    }
  }
}