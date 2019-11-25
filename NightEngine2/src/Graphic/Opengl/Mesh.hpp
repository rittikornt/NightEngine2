/*!
  @file Mesh.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Mesh
*/

#pragma once
#include <vector>
#include "Graphic/Opengl/Shader.hpp"
#include "Graphic/Opengl/Vertex.hpp"
#include "Graphic/Opengl/Texture.hpp"
#include "Graphic/Opengl/VertexArrayObject.hpp"

namespace Graphic
{
  class Mesh
  {
    public:
      //! @brief Default Constructor
      Mesh(void) = default;

      //! @brief Constructor for building VAO
      explicit Mesh(const std::vector<Vertex>& vertices
        , const std::vector<unsigned>& indices
        , bool buildNow);

      //! @brief Constructor for building VAO
      explicit Mesh(const Vertex* vertices, size_t vertexArraySize
        , const unsigned* indices, size_t indexArraySize
        , bool buildNow);

      //! @brief Build data into opengl State
      void Build(void);

      //! @brief Build with instance buffer draw (Should only be called in InstanceDrawer)
      void BuildInstancesDraw(size_t dataSize, void* data);

      //! @brief Draw mesh by direct VAO drawcall
      void Draw(void) const;

      //! @brief Draw mesh with option
      void DrawInstanced(size_t amount) const;

      //! @brief Get Mesh Polygon count
      unsigned GetPolygonCount(void) const { return m_polygonCount; }
    private:
      VertexArrayObject    m_vao;
      unsigned             m_verticesCount;
      unsigned             m_polygonCount;
  };
}