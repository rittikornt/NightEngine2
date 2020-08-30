#include "GBuffer.hpp"
/*!
  @file GBuffer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GBuffer
*/

#include "Graphics/Opengl/GBuffer.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"
#include "Graphics/Opengl/Shader.hpp"

#include <glad/glad.h>

using namespace NightEngine;

namespace Rendering
{
  void GBuffer::Init(int width, int height)
  {
    m_width = width, m_height = height;

    m_fbo.Init();

    //Depth Buffer
    m_depthBuffer.Init(width, height);
    m_fbo.AttachRenderBuffer(m_depthBuffer);
    CHECKGL_ERROR();

    // TODO: Replace posWS with Depth texture (viewpos reconstruction instead)
    // (0) vec4(pos.xyz, n.x)
    m_textures[0] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[0], 0);

    //(1) vec4(albedo.xyz, metallic)
    m_textures[1] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::SRGB8_ALPHA8, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[1], 1);

    //(2) vec4(lightSpacePos, n.y)
    m_textures[2] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[2], 2);

    //(3) vec4(emissive.xyz, roughness)
    m_textures[3] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA12, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[3], 3);

    //Setup multiple render target
    m_fbo.SetupMultipleRenderTarget();

    m_fbo.Bind();
  }

  void GBuffer::Bind(void)
  {
    m_fbo.Bind();
  }

  void GBuffer::Unbind(void)
  {
    m_fbo.Unbind();
  }

  void GBuffer::BindTextures(void)
  {
    int count = static_cast<size_t>(GBufferTarget::Count);
    for (int i = 0; i < count; ++i)
    {
      m_textures[i].BindToTextureUnit(i);
    }
  }

  Texture& GBuffer::GetTexture(GBufferTarget target)
  {
    return m_textures[static_cast<size_t>(target)];
  }

  Texture& GBuffer::GetTexture(size_t index)
  {
    return m_textures[index];
  }

  void GBuffer::CopyDepthBufferTo(unsigned fboId)
  {
    m_fbo.CopyBufferToTarget(m_width, m_height, m_width, m_height
      , fboId, GL_DEPTH_BUFFER_BIT);
  }

  void GBuffer::RefreshTextureUniforms(Shader& shader)
  {
    //Gbuffer's texture
    shader.SetUniform("u_gbuffer.gbuffer0"
      , 0);
    shader.SetUniform("u_gbuffer.gbuffer1"
      , 1);
    shader.SetUniform("u_gbuffer.gbuffer2"
      , 2);
    shader.SetUniform("u_gbuffer.gbuffer3"
      , 3);
  }
}