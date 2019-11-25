/*!
  @file Vertex.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Vertex
*/
#include "Graphic/Opengl/Vertex.hpp"
#include <glm/mat4x4.hpp>

namespace Graphic
{
  const AttributePointerInfo Vertex::s_attributePointerInfo
    = { 5
    , std::vector<unsigned>{3, 3, 2, 3, 16}
    , std::vector<size_t>{sizeof(glm::vec3), sizeof(glm::vec3)
      ,sizeof(glm::vec2), sizeof(glm::vec3) ,sizeof(glm::mat4)}
    , std::vector<bool>{ false,false,false ,true, false} 
    , std::vector<int>{ 0, 0, 0, 0, 1} };
}