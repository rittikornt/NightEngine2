/*!
  @file GBuffer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GBuffer
*/

#include "Graphics/Opengl/GBuffer.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

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

    //Position color buffer
    m_textures[0] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[0], 0);

    //Normal color buffer
    m_textures[1] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[1], 1);

    //Albedo color buffer
    m_textures[2] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[2], 2);

    //RoughnessMetallic color buffer
    m_textures[3] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[3], 3);

    //Emissive color buffer
    m_textures[4] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGB16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[4], 4);

    //LightSpacePosition color buffer
    m_textures[5] = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_fbo.AttachTexture(m_textures[5], 5);

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
}