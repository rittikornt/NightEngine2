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

namespace Rendering
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

      //History RT
      m_historyRT = Texture::GenerateRenderTexture(width, height
        , Texture::Format::RGBA16F, Texture::Format::RGBA
        , Texture::FilterMode::LINEAR
        , Texture::WrapMode::CLAMP_TO_EDGE);
      m_historyRT.SetName("SceneHistoryColorRT");

      RefreshTextureUniforms();
    }

    void TAA::Apply(VertexArrayObject& screenVAO
      , GBuffer& gbuffer, Texture& screenTexture
      , FrameBufferObject& sceneFbo, const CameraObject& cam)
    {
      sceneFbo.Bind();
      {
        m_TAAShader.Bind();
        {
          screenTexture.BindToTextureUnit(0);
          m_historyRT.BindToTextureUnit(1);
          gbuffer.m_depthTexture.BindToTextureUnit(2);
          gbuffer.GetTexture(GBufferTarget::MotionVector).BindToTextureUnit(3);

          m_TAAShader.SetUniform("u_TAAFrameInfo", glm::vec4(1.0f, cam.m_taaFrameIndex
            , cam.m_activeJitteredUV.x, cam.m_activeJitteredUV.y));

          //Draw Quad
          screenVAO.Draw();
        }
        m_TAAShader.Unbind();
      }
      sceneFbo.Unbind();

      //Save history buffer
      sceneFbo.CopyToTexture(m_historyRT
        , m_width, m_height);
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