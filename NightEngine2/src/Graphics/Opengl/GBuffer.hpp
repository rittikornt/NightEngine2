/*!
  @file GBuffer.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GBuffer
*/
#pragma once
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/RenderBufferObject.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/Shader.hpp"

namespace Rendering
{
  enum class GBufferTarget: size_t
  {
    Position = 0,
    Normal,
    Albedo,
    RoughnessMetallic,
    Emissive,
    LightSpacePosition,
    Count
  };
  //TODO: Calculate Position from Depth Buffer

  //! @brief GBuffer struct
  struct GBuffer
  {
    FrameBufferObject   m_fbo;
    Texture             m_textures[static_cast<size_t>(GBufferTarget::Count)];
    RenderBufferObject  m_depthBuffer;
    int                 m_width  = 1;
    int                 m_height = 1;

    //! @brief Initialize G buffer
    void Init(int width, int height)
    {
      m_width = width, m_height = height;

      m_fbo.Init();

      //Depth Buffer
      m_depthBuffer.Init(width, height);
      m_fbo.AttachRenderBuffer(m_depthBuffer);
      CHECKGL_ERROR();

      //Position color buffer
      m_textures[0] = Texture::GenerateNullTexture(width, height
        , Texture::Channel::RGB16F, Texture::Channel::RGB
        , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
      m_fbo.AttachTexture(m_textures[0], 0);

      //Normal color buffer
      m_textures[1] = Texture::GenerateNullTexture(width, height
        , Texture::Channel::RGB16F, Texture::Channel::RGB
        , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
      m_fbo.AttachTexture(m_textures[1], 1);

      //Albedo color buffer
      m_textures[2] = Texture::GenerateNullTexture(width, height
        , Texture::Channel::RGB, Texture::Channel::RGB
        , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
      m_fbo.AttachTexture(m_textures[2], 2);

      //RoughnessMetallic color buffer
      m_textures[3] = Texture::GenerateNullTexture(width, height
        , Texture::Channel::RGB16F, Texture::Channel::RGB
        , Texture::FilterMode::NEAREST, Texture::WrapMode::CLAMP_TO_EDGE);
      m_fbo.AttachTexture(m_textures[3], 3);

      //Emissive color buffer
      m_textures[4] = Texture::GenerateNullTexture(width, height
        , Texture::Channel::RGB16F, Texture::Channel::RGB
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

    //! @brief Bind to fbo
    void Bind(void)
    {
      m_fbo.Bind();
    }

    //! @brief Unbind fbo
    void Unbind(void)
    {
      m_fbo.Unbind();
    }

    //! @brief Bind all textures to its corresponding Textureunit
    void BindTextures(void)
    {
      int count = static_cast<size_t>(GBufferTarget::Count);
      for (int i = 0; i < count; ++i)
      {
        m_textures[i].BindToTextureUnit(i);
      }
    }

    //! @brief Get Texture
    Texture& GetTexture(GBufferTarget target)
    {
      return m_textures[static_cast<size_t>(target)];
    }

    //! @brief Get Texture
    Texture& GetTexture(size_t index)
    {
      return m_textures[index];
    }

    //! @brief Copy Depth buffer to fbo target (0 for screen fbo)
    void CopyDepthBufferTo(unsigned fboId)
    {
      m_fbo.CopyBufferToTarget(m_width, m_height, m_width, m_height
        , fboId, GL_DEPTH_BUFFER_BIT);
    }
  };
}