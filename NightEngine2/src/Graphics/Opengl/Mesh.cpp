/*!
  @file Mesh.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Mesh
*/

#include "Graphics/Opengl/Mesh.hpp"

namespace Rendering
{
  Mesh::Mesh(const std::vector<Vertex>& vertices
    , const std::vector<unsigned>& indices, bool buildNow)
  {
    m_verticesCount = vertices.size();
    m_polygonCount = (indices.size() / sizeof(unsigned)) / 3;

    if (buildNow)
    {
      m_vao.Init();
      m_vao.Build(BufferMode::Static, vertices
        , indices);
    }
    else
    {
      m_vao.FillData(vertices, indices);
    }
  }

  Mesh::Mesh(const Vertex * vertices, size_t vertexArraySize
    , const unsigned * indices, size_t indexArraySize
    , bool buildNow)
  {
    m_verticesCount = vertexArraySize / sizeof(Vertex);
    m_polygonCount = (indexArraySize/ sizeof(unsigned) ) / 3;

    if (buildNow)
    {
      m_vao.Init();
      m_vao.Build(BufferMode::Static, vertices, vertexArraySize
        , indices, indexArraySize);
    }
    else
    {
      m_vao.FillData(vertices, vertexArraySize
        , indices, indexArraySize);
    }
  }

  void Mesh::Build(void)
  {
    m_vao.Init();
    m_vao.Build(BufferMode::Static);
  }

  void Mesh::BuildInstancesDraw(size_t dataSize, void* data)
  {
    m_vao.Init();
    m_vao.InitInstanceDraw(dataSize, data);

    m_vao.Build(BufferMode::Static);
  }

  void Mesh::Draw(void) const
  {
    m_vao.Draw();
  }

  void Mesh::DrawInstanced(size_t amount) const
  {
    m_vao.DrawInstanced(amount);
  }

  void Mesh::Release(void)
  {
    m_vao.Release();
  }
}