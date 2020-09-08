/*!
  @file FXAA.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FXAA
*/
#include "Graphics/Opengl/RenderPass/CameraMotionVector.hpp"

#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"

#include "Graphics/Opengl/RenderPass/GBuffer.hpp"
#include "Graphics/Opengl/CameraObject.hpp"

namespace Rendering
{
  namespace Prepass
  {
    INIT_REFLECTION_FOR(CameraMotionVector)

    void CameraMotionVector::Init(int width, int height, GBuffer& gbuffer)
    {
      INIT_POSTPROCESSEFFECT();

      m_cmvShader.Create();
      m_cmvShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_cmvShader.AttachShaderFile("RenderPass/Prepass/camera_motion_vector.frag");
      m_cmvShader.Link();

      //FBO
      m_fbo.Init();
      m_fbo.AttachColorTexture(gbuffer.GetTexture(4));
      m_fbo.Bind();
      m_fbo.Unbind();

      RefreshTextureUniforms();

      m_isFirstFrame = true;
    }

    void CameraMotionVector::Render(VertexArrayObject& screenVAO
      , GBuffer& gbuffer, CameraObject& cam)
    {
      const auto& unjitteredVP = cam.m_unjitteredVP;
      auto& prevUnjitteredVP = cam.m_prevUnjitteredVP;
      if (m_isFirstFrame)
      {
        prevUnjitteredVP = unjitteredVP;
        m_isFirstFrame = false;
      }

      m_fbo.Bind();
      {
        m_cmvShader.Bind();
        {
          gbuffer.GetTexture(GBufferTarget::PositionAndNormalX).BindToTextureUnit(0);

          m_cmvShader.SetUniform("u_prevUnJitteredVP", prevUnjitteredVP);
          m_cmvShader.SetUniform("u_unjitteredVP", unjitteredVP);

          //Draw Quad
          screenVAO.Draw();
        }
        m_cmvShader.Unbind();
      }
      m_fbo.Unbind();
    }

    void CameraMotionVector::RefreshTextureUniforms(void)
    {
      m_cmvShader.Bind();
      {
        m_cmvShader.SetUniform("gbuffer0", 0);
      }
      m_cmvShader.Unbind();
    }
  }
}