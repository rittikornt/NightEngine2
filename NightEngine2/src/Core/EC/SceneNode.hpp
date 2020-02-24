/*!
  @file SceneNode.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SceneNode
*/

#pragma once
#include <unordered_set>

#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

namespace NightEngine
{
  namespace EC
  {
    namespace SceneManager
    {
      //! @brief SceneNode Serialize function
      JsonValue SerializeSceneNode(Reflection::Variable& variable);

      //! @brief SceneNode Deserialize function
      void DeserializeSceneNode(ValueObject& valueObject, Reflection::Variable& variable);
    }

    struct SceneNode
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER(SceneNode, true
          , SceneManager::SerializeSceneNode, SceneManager::DeserializeSceneNode)
          .MR_ADD_MEMBER_PRIVATE(SceneNode, m_parentIndex, true)
          .MR_ADD_MEMBER_PRIVATE(SceneNode, m_children, true);
      }

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