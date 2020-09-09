/*!
  @file FXAA.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FXAA
*/
#include "Graphics/Opengl/Postprocess/FXAA.hpp"

#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"

namespace Rendering
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(FXAA)

    void FXAA::Init(int width, int height)
    {
      INIT_POSTPROCESSEFFECT();
      m_fxaaShader.Create();
      //m_fxaaShader.AttachShaderFile("Postprocess/fxaa.vert");
      //m_fxaaShader.AttachShaderFile("Postprocess/fxaa_triangle.vert");
      m_fxaaShader.AttachShaderFile("Utility/fullscreenTriangle.vert");
      m_fxaaShader.AttachShaderFile("Postprocess/fxaa.frag");
      m_fxaaShader.Link();

      RefreshTextureUniforms();
    }

    void FXAA::Apply(VertexArrayObject& screenVAO
      , Texture& screenTexture, FrameBufferObject& sceneFbo)
    {
      sceneFbo.Bind();
      {
        m_fxaaShader.Bind();
        {
          screenTexture.BindToTextureUnit(0);

          //Draw Quad
          screenVAO.Draw();
        }
        m_fxaaShader.Unbind();
      }
      sceneFbo.Unbind();
    }

    void FXAA::ApplyToScreen(VertexArrayObject& screenVAO
      , Texture& screenTexture, float zoomScale)
    {
      m_fxaaShader.Bind();
      {
        m_fxaaShader.SetUniform("u_scale", zoomScale);

        screenTexture.BindToTextureUnit(0);

        //Draw Quad
        screenVAO.Draw();
      }
      m_fxaaShader.Unbind();
    }

    void FXAA::RefreshTextureUniforms(void)
    {
      m_fxaaShader.Bind();
      {
        m_fxaaShader.SetUniform("u_screenTexture", 0);
      }
      m_fxaaShader.Unbind();
    }
  }
}