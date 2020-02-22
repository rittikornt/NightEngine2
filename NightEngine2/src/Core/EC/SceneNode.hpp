/*!
  @file SceneNode.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SceneNode
*/

#pragma once
#include "Core/Container/Vector.hpp"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"

namespace NightEngine
{
  namespace EC
  {
    struct SceneNode
    {
      explicit SceneNode(Handle<GameObject> gameObject
        , int reserveSize) : m_gameObject(gameObject)
      {
        m_sceneNodes.reserve(reserveSize);
      }

      Handle<GameObject> m_gameObject;
      NightEngine::Container::Vector<SceneNode> m_sceneNodes;
    };
  }
}