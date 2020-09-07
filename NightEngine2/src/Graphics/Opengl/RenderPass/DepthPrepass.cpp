#include "GBuffer.hpp"
/*!
  @file DepthPrepass.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of DepthPrepass
*/

#include "Graphics/Opengl/RenderPass/DepthPrepass.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include <glad/glad.h>

using namespace NightEngine;

namespace Rendering
{
  void DepthPrepass::Init(int width, int height, const Texture& depthTexture)
  {
    m_width = width, m_height = height;

    //Material
    m_depthPrepassMaterial.InitShader("ShaderPass/depth_prepass.vert"
      , "ShaderPass/depth_prepass.frag");

    //FBO
    m_fbo.Init();
    m_fbo.AttachDepthTexture(depthTexture);
    CHECKGL_ERROR();

    m_fbo.Bind();
    m_fbo.Unbind();

    RefreshTextureUniforms();
  }

  void DepthPrepass::Bind(void)
  {
    m_fbo.Bind();
  }

  void DepthPrepass::Unbind(void)
  {
    m_fbo.Unbind();
  }

  void DepthPrepass::RefreshTextureUniforms()
  {
  }
}