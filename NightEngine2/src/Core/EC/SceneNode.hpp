/*!
  @file SceneNode.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SceneNode
*/

#pragma once
#include <unordered_set>

#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"

namespace NightEngine
{
  namespace EC
  {
    struct SceneNode
    {
      inline void Reserve(int childCount)
      {
        m_children.reserve(childCount);
      }

      inline void AddChild(int childIndex)
      {
        m_children.insert(childIndex);
      }

      inline void SetParent(int parentIndex)
      {
        m_parentIndex = parentIndex;
      }

      int m_parentIndex = -1;
      std::unordered_set<int> m_children;
    };
  }
}