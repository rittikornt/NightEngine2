/*!
  @file RenderBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderBufferObject
*/
#include "Graphics/Opengl/RenderBufferObject.hpp"
#include "Graphics/Opengl/OpenglAllocationTracker.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

using namespace NightEngine;

namespace Rendering
{
  static void ReleaseRBOID(GLuint shaderID)
  {
    //TODO: This delete need ref count too
    glDeleteRenderbuffers(1, &shaderID);
    DECREMENT_ALLOCATION(RenderBufferObject, shaderID);
    CHECKGL_ERROR();
  }

  REGISTER_DEALLOCATION_FUNC(RenderBufferObject, ReleaseRBOID)

  /////////////////////////////////////////////////////////////////////////

  RenderBufferObject::~RenderBufferObject(void)
  {
    CHECK_LEAK(RenderBufferObject, m_id);
    //if (m_id != (~0))
    //{
    //  //TODO: This delete need ref count too
    //  glDeleteRenderbuffers(1, &m_id);
    //  DECREMENT_ALLOCATION(RenderBufferObject, m_id);
    //}
  }

  void RenderBufferObject::RenderBufferObject::Init(int width, int height
    , Format format)
  {
    glGenRenderbuffers(1, &m_id);
    INCREMENT_ALLOCATION(RenderBufferObject, m_id);
    SetBuffer(width, height, format);
  }

  void RenderBufferObject::SetBuffer(int width, int height, Format format)
  {
    m_format = format;
    
    Bind();
    {
      glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(format)
        , width, height);
      CHECKGL_ERROR();
    }
    Unbind();
  }
  
  void RenderBufferObject::Bind(void)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, m_id);
  }

  void RenderBufferObject::Unbind(void)
  {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }
}