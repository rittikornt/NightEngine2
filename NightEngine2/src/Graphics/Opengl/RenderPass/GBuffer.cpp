#include "GBuffer.hpp"
/*!
  @file GBuffer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GBuffer
*/

#include "Graphics/Opengl/RenderPass/GBuffer.hpp"

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
    m_depthTexture = m_fbo.CreateAndAttachDepthStencilTexture(width, height);
    m_depthTexture.SetName("DepthPrepass RT");
    CHECKGL_ERROR();

    // TODO: Replace posWS with Depth texture (viewpos reconstruction instead)
    // (0) vec4(pos.xyz, n.x)
    m_textures[0] = Texture::GenerateRenderTexture(width, height
      , Texture::Format::RGBA16F, Texture::Format::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_textures[0].SetName("GBuffer0 (Pos.xyz, N.x)");
    m_fbo.AttachColorTexture(m_textures[0], 0);

    //(1) vec4(albedo.xyz, metallic)
    m_textures[1] = Texture::GenerateRenderTexture(width, height
      , Texture::Format::SRGB8_ALPHA8, Texture::Format::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_textures[1].SetName("GBuffer1 (albedo.xyz, metallic)");
    m_fbo.AttachColorTexture(m_textures[1], 1);

    //(2) vec4(lightSpacePos, n.y)
    m_textures[2] = Texture::GenerateRenderTexture(width, height
      , Texture::Format::RGBA16F, Texture::Format::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_textures[2].SetName("GBuffer2 (lightSpacePos, n.y)");
    m_fbo.AttachColorTexture(m_textures[2], 2);

    //(3) vec4(emissive.xyz, roughness)
    m_textures[3] = Texture::GenerateRenderTexture(width, height
      , Texture::Format::RGBA12, Texture::Format::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_textures[3].SetName("GBuffer3 (emissive.xyz, roughness)");
    m_fbo.AttachColorTexture(m_textures[3], 3);

    // (4) vec2(motionvector.xy);
    m_textures[4] = Texture::GenerateRenderTexture(width, height
      , Texture::Format::RG16F, Texture::Format::RGBA
      , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
    m_textures[4].SetName("GBuffer4 (motionvector.xy)");
    m_fbo.AttachColorTexture(m_textures[4], 4);

    //Setup multiple render target
    m_fbo.SetupMultipleRenderTarget();

    m_fbo.Bind();
    m_fbo.Unbind();
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