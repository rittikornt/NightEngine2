/*!
  @file UniformBufferObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of UniformBufferObject
*/

#include "Graphic/UniformBufferObject.hpp"

#include "Core/Macros.hpp"

#include <glad/glad.h>

namespace Graphic
{
  UniformBufferObject::~UniformBufferObject(void)
  {
    if (m_id != ~(0))
    {
      glDeleteBuffers(1, &m_id);
    }
  }

  void UniformBufferObject::Init(std::size_t size, unsigned bufferPointIndex)
  {
    glGenBuffers(1, &m_id);

    Bind();
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    Unbind();
    
    glBindBufferRange(GL_UNIFORM_BUFFER, bufferPointIndex, m_id, 0, size);
    CHECKGL_ERROR();
  }

  void UniformBufferObject::Bind(void)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, m_id);
  }

  void UniformBufferObject::Unbind(void)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void UniformBufferObject::FillBuffer(std::size_t offset, std::size_t dataSize, void* data)
  {
    Bind();
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
    Unbind();
  }
  
}