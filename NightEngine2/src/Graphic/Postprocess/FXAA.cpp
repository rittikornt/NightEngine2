/*!
  @file FXAA.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FXAA
*/
#include "Graphic/Postprocess/FXAA.hpp"

#include "Graphic/Texture.hpp"
#include "Graphic/FrameBufferObject.hpp"
#include "Graphic/VertexArrayObject.hpp"

namespace Graphic
{
  namespace Postprocess
  {
    void FXAA::Init(int width, int height)
    {
      m_fxaaShader.Init();
      m_fxaaShader.AttachShaderFile("Postprocess/fxaa.vert");
      m_fxaaShader.AttachShaderFile("Postprocess/fxaa.frag");
      m_fxaaShader.Link();

      m_resolution = glm::vec2(width, height);
    }

    void FXAA::Apply(VertexArrayObject& screenQuad
      , Texture& screenTexture, FrameBufferObject& sceneFbo)
    {
      sceneFbo.Bind();
      {
        m_fxaaShader.Bind();
        {
          m_fxaaShader.SetUniform("u_resolution", m_resolution);
          m_fxaaShader.SetUniform("u_screenTexture", 0);

          screenTexture.BindToTextureUnit(0);

          //Draw Quad
          screenQuad.Draw();
        }
        m_fxaaShader.Unbind();
      }
      sceneFbo.Unbind();
    }

    void FXAA::ApplyToScreen(VertexArrayObject & screenQuad, Texture & screenTexture)
    {
      m_fxaaShader.Bind();
      {
        m_fxaaShader.SetUniform("u_resolution", m_resolution);
        m_fxaaShader.SetUniform("u_screenTexture", 0);

        screenTexture.BindToTextureUnit(0);

        //Draw Quad
        screenQuad.Draw();
      }
      m_fxaaShader.Unbind();
    }
  }
}