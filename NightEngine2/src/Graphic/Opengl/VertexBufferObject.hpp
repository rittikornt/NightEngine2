/*!
  @file VertexBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of VertexBufferObject
*/
#pragma once
// System Headers
#include <glad/glad.h>

#include <vector>

namespace Graphic
{
	//Forward declaration
  struct Vertex;
  enum class DrawMode : GLenum;

	enum class BufferMode : GLenum
	{
		Static = GL_STATIC_DRAW,
		Dynamic = GL_DYNAMIC_DRAW,
		Stream = GL_STREAM_DRAW
  };

	class VertexBufferObject
	{
	public:

    //! @brief Constructor
		VertexBufferObject() :m_objectID(~0){}

    //! @brief Destructor
    ~VertexBufferObject();

    //! @brief Deallocate All VBO Object
    static void ReleaseAllLoadedVBO(void);

    //! @brief Initialization
		void Init();

    //! @brief Fill Vertex with vector of float
    void FillVertex(const std::vector<float>& floatArray);

    //! @brief Fill Vertex with vector
    void FillVertex(const std::vector<Vertex>& vertexArray);

    //! @brief Fill Vertex with float array
    void FillVertex(const float* vertexArray, size_t arraySize);

    //! @brief Fill Vertex with array
    void FillVertex(const Vertex* vertexArray, size_t arraySize);
		
    //! @brief Add Vertex
    void AddVertex(const Vertex& vertex);

    //! @brief Clear Datas
    void Clear(void);

    //! @brief Get Vertices
    const std::vector<float>& GetVertices(void) { return m_vertices; }
		
    //! @brief Build the vertex into opengl buffer
    void Build(BufferMode mode) const;

    //! brief bind VBO
		void Bind() const;

    //! brief unbind VBO
		void Unbind() const;

    //! @brief Draw
    void Draw(DrawMode drawMode) const;

    //! brief Get the opengl ID
		GLuint GetID() { return m_objectID; }
	private:
    GLuint m_objectID;
    std::vector<float> m_vertices;
	};

} // Graphic

