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
#include "Graphics/Opengl/RenderState.hpp"
#include "Graphics/Opengl/DebugMarker.hpp"

#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/InstanceDrawer.hpp"

#include <glad/glad.h>

using namespace NightEngine;

namespace NightEngine::Rendering::Opengl
{
  void GBuffer::LazyInit(CameraObject& cameraObject)
  {
    if (m_width == 0)
    {
      m_width = cameraObject.m_scaledPixelResolution.x, m_height = cameraObject.m_scaledPixelResolution.y;

      m_fbo.Init();

      // Motion vector
      m_motionVector = Texture::GenerateRenderTexture(m_width, m_height
        , Texture::Format::RG16F, Texture::PixelFormat::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
      m_motionVector.SetName("MotionVector (RG16F)");

      //Depth Buffer
      m_depthTexture = m_fbo.CreateAndAttachDepthStencilTexture(m_width, m_height
        , GL_LINEAR, GL_LINEAR);
      m_depthTexture.SetName("DepthPrepass RT");
      CHECKGL_ERROR();

      // (0) vec4(n.xy)
      m_textures[0] = Texture::GenerateRenderTexture(m_width, m_height
        , Texture::Format::RG16F, Texture::PixelFormat::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
      m_textures[0].SetName("GBuffer0 (N.xy)");
      m_fbo.AttachColorTexture(m_textures[0], 0);

      //(1) vec4(albedo.xyz, metallic)
      m_textures[1] = Texture::GenerateRenderTexture(m_width, m_height
        , Texture::Format::SRGB8_ALPHA8, Texture::PixelFormat::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
      m_textures[1].SetName("GBuffer1 (albedo.xyz, metallic)");
      m_fbo.AttachColorTexture(m_textures[1], 1);

      //(2) vec4(emissive.xyz, roughness)
      m_textures[2] = Texture::GenerateRenderTexture(m_width, m_height
        , Texture::Format::RGBA12, Texture::PixelFormat::RGBA
        , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
      m_textures[2].SetName("GBuffer3 (emissive.xyz, roughness)");
      m_fbo.AttachColorTexture(m_textures[2], 2);

      //Setup multiple render target
      m_fbo.SetupMultipleRenderTarget();

      m_fbo.Bind();
      m_fbo.Unbind();
    }
    else
    {
      //Check for Resize
      float width = cameraObject.m_scaledPixelResolution.x, height = cameraObject.m_scaledPixelResolution.y;
      if (width != m_width || height != m_height)
      {
        m_width = width, m_height = height;

        m_motionVector.Resize(width, height, Texture::PixelFormat::RGBA);
        m_depthTexture.Resize(width, height, Texture::PixelFormat::DepthStencil, GL_UNSIGNED_INT_24_8);

        m_textures[0].Resize(width, height, Texture::PixelFormat::RGBA);
        m_textures[1].Resize(width, height, Texture::PixelFormat::RGBA);
        m_textures[2].Resize(width, height, Texture::PixelFormat::RGBA);
      }
    }
  }

  void GBuffer::Execute(NightEngine::EC::Handle<Material>& defaultMaterial
    , DrawOpaqueFn drawOpaquePassFn)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glEnable(GL_STENCIL_TEST);
    RenderSetup::WriteStencilAlways(RenderFeature::GBUFFER_MASK);

    DebugMarker::PushDebugGroup("GBuffer Pass");
    m_fbo.Bind();
    {
      //Clear Buffer
      glClear(GL_COLOR_BUFFER_BIT);

      drawOpaquePassFn(defaultMaterial);
    }
    m_fbo.Unbind();
    DebugMarker::PopDebugGroup();
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
      , fboId, GL_DEPTH_BUFFER_BIT, GL_LINEAR);
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
  }
}