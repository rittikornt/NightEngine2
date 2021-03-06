/*!
  @file FrameBufferObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FrameBufferObject
*/

#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/RenderBufferObject.hpp"
#include "Graphics/Opengl/Cubemap.hpp"

#include "Graphics/Opengl/OpenglAllocationTracker.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include <glad/glad.h>

using namespace NightEngine;

namespace NightEngine::Rendering::Opengl
{
  static void ReleaseFBOID(GLuint shaderID)
  {
    glDeleteFramebuffers(1, &shaderID);
    DECREMENT_ALLOCATION(FrameBufferObject, shaderID);
    CHECKGL_ERROR();
  }

  REGISTER_DEALLOCATION_FUNC(FrameBufferObject, ReleaseFBOID)

  /////////////////////////////////////////////////////////////////////////

  FrameBufferObject::~FrameBufferObject(void)
  {
    CHECK_LEAK(FrameBufferObject, m_id);
    //if (m_id != ~(0))
    //{
    //  glDeleteFramebuffers(1, &m_id);
    //  DECREMENT_ALLOCATION(FrameBufferObject, m_id);
    //  CHECKGL_ERROR();
    //}
  }

  void FrameBufferObject::Init(void)
  {
    glGenFramebuffers(1, &m_id);
    INCREMENT_ALLOCATION(FrameBufferObject, m_id);
    CHECKGL_ERROR();
  }
  
  void FrameBufferObject::Bind(void)
  {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      Debug::Log << Logger::MessageType::ERROR_MSG
        << "Attempt to bind to an incomplete FrameBuffer\n";
      ASSERT_TRUE(false);
    }
    BindUnsafe();
  }

  void FrameBufferObject::Unbind(void)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CHECKGL_ERROR();
  }

  void FrameBufferObject::SetupMultipleRenderTarget(void)
  {
    BindUnsafe();
    {
      glDrawBuffers((GLsizei)m_renderTarget.size(), &m_renderTarget[0]);
    }
    Unbind();
  }

  void FrameBufferObject::AttachCubemap(Cubemap& cubemap)
  {
    BindUnsafe();

    //Attach cubemap to fbo
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT
      , cubemap.GetID(), 0);

    //Explicit tell opengl to not render to color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    Unbind();
  }

  void FrameBufferObject::AttachCubemapFace(Cubemap& cubemap, int textureIndex
    , int cubemapIndex, int mipmapLevel)
  {
    //BindUnsafe();

    glFramebufferTexture2D(GL_FRAMEBUFFER
      , GL_COLOR_ATTACHMENT0 + textureIndex
      , GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubemapIndex
      , cubemap.GetID(), mipmapLevel);

    //Unbind();
  }

  void FrameBufferObject::AttachColorTexture(const Texture& texture, int textureIndex)
  {
    m_renderTarget.emplace_back(GL_COLOR_ATTACHMENT0 + textureIndex);

    BindUnsafe();

    glFramebufferTexture2D(GL_FRAMEBUFFER
      , GL_COLOR_ATTACHMENT0 + textureIndex, GL_TEXTURE_2D, texture.GetID(), 0);

    Unbind();
  }

  void FrameBufferObject::AttachDepthTexture(const Texture& texture)
  {
    BindUnsafe();

    GLenum attachment = texture.GetInternalFormat() == Texture::Format::Depth24Stencil8 ?
      GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
    glFramebufferTexture2D(GL_FRAMEBUFFER
      , attachment, GL_TEXTURE_2D, texture.GetID(), 0);

    Unbind();
  }

  TextureIdentifier FrameBufferObject::CreateAndAttachDepthStencilTexture(float width, float height
    , int minFilter, int maxFilter)
  {
    //Generate ID
    unsigned int textureID;
    glGenTextures(1, &textureID);
    INCREMENT_ALLOCATION(Texture, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Setup Texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (GLsizei)width, (GLsizei)height, 0,
    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    //GL_NEAREST, GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    BindUnsafe();
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
    Unbind();

    return TextureIdentifier{ textureID, "Depth24Stencil8", "", GL_DEPTH24_STENCIL8 , GL_NEAREST };
  }

  TextureIdentifier FrameBufferObject::CreateAndAttachDepthTexture(int width, int height)
  {
    //Generate ID
    unsigned int textureID;
    glGenTextures(1, &textureID);
    INCREMENT_ALLOCATION(Texture, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Setup Texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)width, (GLsizei)height, 0,
      GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    BindUnsafe();
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    Unbind();

    return TextureIdentifier{ textureID, "Depth24", "", GL_DEPTH_COMPONENT , GL_NEAREST };
  }

  void FrameBufferObject::AttachRenderBuffer(RenderBufferObject& rbo)
  {
    BindUnsafe();
    {
      //Mode based on RBO's format
      GLenum mode = rbo.GetFormat() == RenderBufferObject::Format::DEPTH24 ?
        GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;

      //Attach RBO
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, mode
        , GL_RENDERBUFFER, rbo.GetID());

      CHECKGL_ERROR();
    }
    Unbind();
  }

  void FrameBufferObject::CopyBufferToTarget(int width,int height
    , int targetWidth, int targetHeight
    , unsigned fboId, unsigned bitField, int filter)
  {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);

    //bitField: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT
    //filter: GL_NEAREST, GL_LINEAR
    glBlitFramebuffer(0, 0, width, height, 0, 0
      , targetWidth, targetHeight, bitField, filter);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBufferObject::CopyToTexture(Texture& target
    , int newWidth, int newHeight)
  {
    BindUnsafe();
    {
      target.Bind();
      {
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0
          , 0, 0
          , 0, 0, newWidth, newHeight);
      }
      target.Unbind();
    }
    Unbind();
  }

  void FrameBufferObject::BindUnsafe(void)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
  }
}