/*!
  @file Vertex.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Vertex
*/
#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <vector>

namespace Graphic
{
  //! @brief Info for describing Attribute
  struct AttributePointerInfo
  {
    unsigned              m_attributeCount; //Amount of attribute
    std::vector<unsigned> m_dimension;      //How many float per attribute
    std::vector<size_t>   m_size;           //Size of each attribute
    std::vector<bool>     m_normalized;     //Should this attribute be normalized?
    std::vector<int>      m_divisor;        //1 for instance, 0 for normal

    //! @brief Constructor
    AttributePointerInfo(unsigned count, const std::vector<unsigned>& dimension
      , const std::vector<size_t>& sizes, const std::vector<bool>& normalized)
      : m_attributeCount(count), m_dimension(dimension)
      , m_size(sizes), m_normalized(normalized)
      , m_divisor(count,0)
    {
    }

    //! @brief Constructor
    AttributePointerInfo(unsigned count, const std::vector<unsigned>& dimension
      , const std::vector<size_t>& sizes, const std::vector<bool>& normalized
      , const std::vector<int>& divisor)
      : m_attributeCount(count), m_dimension(dimension)
      , m_size(sizes), m_normalized(normalized)
      , m_divisor(divisor)
    {
    }

    //! @brief Get Stride Size
    size_t GetStrideSize()
    {
      size_t sum = 0;
      for (int i = 0; i < m_attributeCount; ++i)
      {
        //Only normal attribute stay in the same buffer
        if (m_divisor[i] == 0)
        {
          sum += m_size[i];
        }
      }
      return sum;
    }
  };

  //! @brief Default Vertex Object
	struct Vertex
	{
    //To Add Attribute, VAO.AddVertex() need to be modify
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_texCoord;
    glm::vec3 m_tangent;

		//Static Description of AttributePointer
    static const AttributePointerInfo s_attributePointerInfo;
	};

}