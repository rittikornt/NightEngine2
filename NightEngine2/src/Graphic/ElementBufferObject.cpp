/*!
  @file ElementBufferObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ElementBufferObject
*/
#include "Graphic/ElementBufferObject.hpp"
#include "Graphic/VertexArrayObject.hpp"
#include "Core/Macros.hpp"

namespace Graphic
{
	ElementBufferObject::DrawMethod ElementBufferObject::s_drawMode = ElementBufferObject::DrawMethod::FILL;

	void ElementBufferObject::SetDrawMethod(DrawMethod mode)
	{
		s_drawMode = mode;
		glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(s_drawMode));
	}

	ElementBufferObject::~ElementBufferObject()
	{
		if (!(m_objectID & (~0)))
		{
			glDeleteBuffers(1, &m_objectID);
		}
	}

	void ElementBufferObject::Init()
	{
		glGenBuffers(1, &m_objectID);
	}

  void ElementBufferObject::FillIndex(const std::vector<unsigned>& indexArray)
  {
    for (size_t i = 0; i < indexArray.size(); ++i)
    {
      AddIndex(indexArray[i]);
    }
  }

	void ElementBufferObject::FillIndex(const unsigned * indexArray, size_t arraySize)
	{
		size_t count = arraySize / sizeof(unsigned);

		for (size_t i = 0; i < count; ++i)
		{
			AddIndex(indexArray[i]);
		}
	}

	void ElementBufferObject::AddIndex(const unsigned & index)
	{
		m_indices.emplace_back(index);
	}

	void ElementBufferObject::Build(BufferMode mode) const
	{
		Bind();

		//Send data buffer to GPU
		size_t size = m_indices.size();
		if (size > 0)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER
				, size * sizeof(unsigned), &m_indices[0]
				, static_cast<GLenum>(mode));
		}

		CHECKGL_ERROR();
	}

	void ElementBufferObject::Draw(DrawMode drawMode) const
	{
		glDrawElements(static_cast<GLenum>(drawMode), m_indices.size()
			, GL_UNSIGNED_INT, 0);
	}

  void ElementBufferObject::DrawInstanced(size_t amount) const
  {
    glDrawElementsInstanced(GL_TRIANGLES, m_indices.size()
      , GL_UNSIGNED_INT, 0, amount);
  }

	void ElementBufferObject::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_objectID);
	}

	void ElementBufferObject::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
} // Graphic
