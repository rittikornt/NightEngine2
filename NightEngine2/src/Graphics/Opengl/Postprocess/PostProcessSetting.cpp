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

namespace Rendering
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(PostProcessSetting)

    void PostProcessSetting::Init(int width, int height)
    {
      //Resources
      m_uberPostMaterial.InitShader("Utility/fullscreenTriangle.vert"
        , "Postprocess/uberpost.frag");

      //Postfx
      m_ppUtility.Init(width, height);

      //Bloom
      m_bloomPP.Init(width, height);

      //SSAO
      m_ssaoPP.Init(width, height);

      //FXAA
      m_fxaaPP.Init(width, height);

      //TAA
      m_taaPP.Init(width, height);

      m_postProcessEffects.emplace_back(&m_bloomPP);
      m_postProcessEffects.emplace_back(&m_ssaoPP);
      m_postProcessEffects.emplace_back(&m_fxaaPP);
      m_postProcessEffects.emplace_back(&m_taaPP);
    }

    void PostProcessSetting::Apply(const PostProcessContext& context)
    {
      CameraObject* camera = context.camera;
      auto screenSize = camera->GetScreenSize();

      GBuffer* gbuffer = context.gbuffer;
      FrameBufferObject* sceneFBO = context.sceneFBO;
      VertexArrayObject* screenVAO = context.screenVAO;
      Texture* screenTexture = context.screenTexture;

      //SSAO
      if (m_ssaoPP.m_enable)
      {
        DebugMarker::PushDebugGroup("SSAO Pass");
        {
          m_ssaoPP.Apply(*screenVAO, *camera
            , *gbuffer, m_ppUtility);
        }
        DebugMarker::PopDebugGroup();
      }
      else
      {
        m_ssaoPP.Clear();
      }

      //TAA
      if (m_taaPP.m_enable)
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
            , m_taaPP.m_enable? m_taaPP.m_currRT: *(context.screenTexture)
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
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

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

            //PP Texture
            {
              if (m_taaPP.m_enable)
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