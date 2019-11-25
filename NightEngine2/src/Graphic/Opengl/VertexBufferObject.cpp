/*!
  @file VertexBufferObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of VertexBufferObject
*/
#include "Graphic/Opengl/VertexBufferObject.hpp"

#include "Graphic/Opengl/VertexArrayObject.hpp"
#include "Graphic/Opengl/Vertex.hpp"
#include "Core/Macros.hpp"

namespace Graphic
{
  VertexBufferObject::~VertexBufferObject()
  {
		if (!(m_objectID & (~0)))
		{
			glDeleteBuffers(1, &m_objectID);
		}
  }

	void VertexBufferObject::Init()
	{
		glGenBuffers(1, &m_objectID);
	}

  void VertexBufferObject::FillVertex(const std::vector<float>& floatArray)
  {
    m_vertices.reserve(floatArray.size());
    for (size_t i = 0; i < floatArray.size(); ++i)
    {
      m_vertices.emplace_back(floatArray[i]);
    }
  }

  void VertexBufferObject::FillVertex(const std::vector<Vertex>& vertexArray)
  {
    //Calculate amount of float in a Vertex
    auto info = Vertex::s_attributePointerInfo;
    auto floatCount = info.GetStrideSize()/(sizeof(float));
   
    m_vertices.reserve(vertexArray.size() * floatCount);
    for (size_t i = 0; i < vertexArray.size(); ++i)
    {
      AddVertex(vertexArray[i]);
    }
  }

  void VertexBufferObject::FillVertex(const float* floatArray, size_t arraySize)
  {
    size_t count = arraySize / sizeof(float);

    for (size_t i = 0; i < count; ++i)
    {
      m_vertices.emplace_back(floatArray[i]);
    }
  }

	void VertexBufferObject::FillVertex(const Vertex* vertexArray, size_t arraySize)
	{
		size_t count = arraySize / sizeof(Vertex);

		for (size_t i = 0; i < count; ++i)
		{
			AddVertex(vertexArray[i]);
		}
	}

	void VertexBufferObject::AddVertex(const Vertex & vertex)
	{
		m_vertices.emplace_back(vertex.m_position.x);
    m_vertices.emplace_back(vertex.m_position.y);
    m_vertices.emplace_back(vertex.m_position.z);

    m_vertices.emplace_back(vertex.m_normal.x);
    m_vertices.emplace_back(vertex.m_normal.y);
    m_vertices.emplace_back(vertex.m_normal.z);

    m_vertices.emplace_back(vertex.m_texCoord.x);
    m_vertices.emplace_back(vertex.m_texCoord.y);

    m_vertices.emplace_back(vertex.m_tangent.x);
    m_vertices.emplace_back(vertex.m_tangent.y);
    m_vertices.emplace_back(vertex.m_tangent.z);
	}

  void VertexBufferObject::Clear(void)
  {
    m_vertices.clear();
  }

	void VertexBufferObject::Build(BufferMode mode) const
	{
		Bind();

		//Send data buffer to GPU
		size_t size = m_vertices.size();
		if (size > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, size * sizeof(float)
				, &m_vertices[0], static_cast<GLenum>(mode));
		}

		CHECKGL_ERROR();
	}

	void VertexBufferObject::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_objectID);
	}

	void VertexBufferObject::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

  void VertexBufferObject::Draw(DrawMode drawMode) const
  {
    glDrawArrays(static_cast<GLenum>(drawMode), 0, m_vertices.size());
  }

} // Graphic