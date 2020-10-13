/*!
  @file TAA.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of TAA
*/
#include "Graphics/Opengl/Postprocess/TAA.hpp"

#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/RenderPass/GBuffer.hpp"
#include "Graphics/Opengl/CameraObject.hpp"

namespace NightEngine::Rendering::Opengl
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(TAA)

    void TAA::Init(int width, int height)
    {
      m_width = width;
      m_height = height;

      INIT_POSTPROCESSEFFECT();
      m_TAAShader.Create();
      m_TAAShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_TAAShader.AttachShaderFile("Postprocess/taa.frag");
      m_TAAShader.Link();

      //RT
      m_currRT = Texture::GenerateRenderTexture(width, height
        , Texture::Format::RGBA16F, Texture::Format::RGBA
        , Texture::FilterMode::LINEAR
        , Texture::WrapMode::CLAMP_TO_EDGE);
      m_currRT.SetName("SceneTAAColorRT");

      m_historyRT = Texture::GenerateRenderTexture(width, height
        , Texture::Format::RGBA16F, Texture::Format::RGBA
        , Texture::FilterMode::LINEAR
        , Texture::WrapMode::CLAMP_TO_EDGE);
      m_historyRT.SetName("SceneHistoryColorRT");

      //FBO
      m_taaFBO.Init();
      m_taaFBO.AttachColorTexture(m_currRT);
      m_taaFBO.Bind();
      m_taaFBO.Unbind();

      m_copyHistoryFBO.Init();
      m_copyHistoryFBO.AttachColorTexture(m_historyRT);
      m_copyHistoryFBO.Bind();
      m_copyHistoryFBO.Unbind();

      RefreshTextureUniforms();

      m_isFirstFrame = true;
    }

    void TAA::Apply(VertexArrayObject& screenVAO
      , GBuffer& gbuffer, Texture& screenTexture
      , FrameBufferObject& sceneFbo, const CameraObject& cam)
    {
      if (m_isFirstFrame)
      {
        sceneFbo.CopyBufferToTarget(m_width, m_height, m_width, m_height
          , m_copyHistoryFBO.GetID(), GL_COLOR_BUFFER_BIT);
        m_isFirstFrame = false;
      }

      m_taaFBO.Bind();
      {
        m_TAAShader.Bind();
        {
          screenTexture.BindToTextureUnit(0);
          m_historyRT.BindToTextureUnit(1);
          gbuffer.m_depthTexture.BindToTextureUnit(2);
          gbuffer.m_motionVector.BindToTextureUnit(3);

          m_TAAShader.SetUniform("u_TAAFrameInfo", glm::vec4(m_sharpness, cam.m_taaFrameIndex
            , cam.m_activeJitteredUV.x, cam.m_activeJitteredUV.y));

          m_TAAShader.SetUniform("u_beforeTonemapping", m_beforeTonemapping);
          
          //Draw Quad
          screenVAO.Draw();
        }
        m_TAAShader.Unbind();
      }
      m_taaFBO.Unbind();

      //Save history buffer
      //sceneFbo.CopyToTexture(m_historyRT
      //  , m_width, m_height);

      m_taaFBO.CopyBufferToTarget(m_width, m_height, m_width, m_height
        , m_copyHistoryFBO.GetID(), GL_COLOR_BUFFER_BIT);
    }

    void TAA::ApplyToScreen(VertexArrayObject& screenVAO
      , Texture& screenTexture)
    {
      m_TAAShader.Bind();
      {
        screenTexture.BindToTextureUnit(0);

        //Draw Quad
        screenVAO.Draw();
      }
      m_TAAShader.Unbind();
    }

    void TAA::RefreshTextureUniforms(void)
    {
      m_TAAShader.Bind();
      {
        m_TAAShader.SetUniform("u_currTexture", 0);
        m_TAAShader.SetUniform("u_prevTexture", 1);

        m_TAAShader.SetUniform("u_depthTexture", 2);
        m_TAAShader.SetUniform("u_motionVectorTexture", 3);
      }
      m_TAAShader.Unbind();
    }
  }
}