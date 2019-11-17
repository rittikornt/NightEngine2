/*!
  @file VertexArrayObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of VertexArrayObject
*/
#pragma once

#include "Graphic/VertexBufferObject.hpp"
#include "Graphic/ElementBufferObject.hpp"

// System Headers
#include <glad/glad.h>
#include <vector>

namespace Graphic
{
	//Forward declaration
	struct Vertex;
  struct AttributePointerInfo;

  enum class DrawMode: GLenum
  {
    TRIANGLES = GL_TRIANGLES,
    LINES = GL_LINES,
    POINTS = GL_POINTS
  };

	class VertexArrayObject
	{
	public:
    //! @brief Constructor
		VertexArrayObject(void) : m_objectID(~0), m_instanceBufferID(~0){}

    //! @brief Constructor
    VertexArrayObject(BufferMode mode
      , const Vertex* vertexArray, size_t vertexArraySize
      , const unsigned* indexArray, size_t indexArraySize);

    //! @brief Destructor
    ~VertexArrayObject(void);

    //! @brief Init, generating opengl object ID
		void Init(void); 

    //! @brief Init and buffer instance draw data
    void InitInstanceDraw(size_t dataSize, void* data);

    /////////////////////////////////////////////////////////

    //! @brief Fill vertices/indices data
    void FillData(const std::vector<float>& floatArray
      , const std::vector<unsigned>& indexArray);

    //! @brief Fill vertices/indices data
    void FillData(const Vertex* vertexArray, size_t vertexArraySize
      , const unsigned* indexArray, size_t indexArraySize);

    //! @brief Fill vertices/indices data
    void FillData(const std::vector<Vertex>& vertexArray
      , const std::vector<unsigned>& indexArray);

    //! @brief Clear Data
    void Clear(void);
    /////////////////////////////////////////////////////////

    //! @brief Build only
    void Build(BufferMode mode);

    //! @brief Build with vector of float
    void Build(BufferMode mode
      , float* floatArray, size_t floatArraySize);

    //! @brief Build with vector of float and AttributePointerInfo
    void Build(BufferMode mode
      , float* floatArray, size_t floatArraySize
      , AttributePointerInfo& attributeInfo);

    //! @brief Build with vector of float and AttributePointerInfo
    void Build(BufferMode mode
      , const std::vector<float>& floatArray
      , const std::vector<unsigned>& indexArray
      , AttributePointerInfo& attributeInfo);

    //! @brief Fill and Build vertices/indices data
    void Build(BufferMode mode
      , const std::vector<Vertex>& vertexArray
      , const std::vector<unsigned>& indexArray);

    //! @brief Fill and Build vertices/indices data
		void Build(BufferMode mode
			, const Vertex* vertexArray, size_t vertexArraySize
			, const unsigned* indexArray, size_t indexArraySize);
    /////////////////////////////////////////////////////////

    //! @brief bind, draw, then unbind
		void Draw(DrawMode drawMode = DrawMode::TRIANGLES) const;

    //! @brief bind, draw, then unbind
    void DrawVBO(DrawMode drawMode = DrawMode::TRIANGLES) const;

    //! @brief bind, draw Instanced, then unbind
    void DrawInstanced(size_t amount) const;

    //! @brief bind
		void Bind(void) const;

    //! @brief unbind
		void Unbind(void) const;

    //! @brief Get opengl objectID
		GLuint GetID(void) { return m_objectID; }
	private:
    //! @brief Setup Attribute Pointer
    void SetupAttributePointer(void);

    //! @brief Setup Attribute Pointer using AttributePointerInfo
    void SetupAttributePointer(AttributePointerInfo& attributeInfo);

    GLuint m_objectID;
    GLuint m_instanceBufferID;
		VertexBufferObject m_vbo;
		ElementBufferObject m_ebo;
	};

} // Graphic

