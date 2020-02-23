/*!
  @file Scene.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Scene
*/

#pragma once
#include "Core/EC/SceneNode.hpp"
#include "Core/Container/Vector.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

namespace NightEngine
{
  namespace EC
  {
    class Scene
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER(Scene, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PRIVATE(Scene, m_active, true);
      }
      public:
        Scene() = default;
        explicit Scene(bool active)
          : m_active(active) {}

        void AddGameObject(Handle<GameObject> gameObject);

        inline const Container::Vector<SceneNode>& GetSceneNodes() const { return m_sceneNodes; }
        inline const Container::Vector<Handle<GameObject>>& GetAllGameObjects() const { return m_sceneGameObjects; }
      private:
        bool m_active = false;
        //TODO: Store set of Components to Update

        //Parallel Arrays of Scene Data
        Container::Vector<SceneNode> m_sceneNodes;
        Container::Vector<Handle<GameObject>> m_sceneGameObjects;
    };
  }
}