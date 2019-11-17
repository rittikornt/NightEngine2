/*!
  @file RenderBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderBufferObject
*/
#include "Graphic/RenderBufferObject.hpp"
#include "Core/Macros.hpp"

namespace Graphic
{
  void RenderBufferObject::RenderBufferObject::Init(int width, int height
    , Format format)
  {
    glGenRenderbuffers(1, &m_id);

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