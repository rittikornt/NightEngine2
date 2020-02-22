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
    void FXAA::Init(int width, int height)
    {
      m_fxaaShader.Create();
      m_fxaaShader.AttachShaderFile("Postprocess/fxaa.vert");
      m_fxaaShader.AttachShaderFile("Postprocess/fxaa.frag");
      m_fxaaShader.Link();

      RefreshTextureUniforms();
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
        screenTexture.BindToTextureUnit(0);

        //Draw Quad
        screenQuad.Draw();
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