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
    namespace SceneManager
    {
      //! @brief Scene Serialize function
      JsonValue SerializeScene(Reflection::Variable& variable);

      //! @brief Scene Deserialize function
      void DeserializeScene(ValueObject& valueObject, Reflection::Variable& variable);
    }

    class Scene
    {
      friend NightEngine::JsonValue SceneManager::SerializeScene(NightEngine::Reflection::Variable&);
      friend void SceneManager::DeserializeScene(NightEngine::ValueObject&, NightEngine::Reflection::Variable&);
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER(Scene, true
          , SceneManager::SerializeScene, SceneManager::DeserializeScene)
          .MR_ADD_MEMBER_PRIVATE(Scene, m_active, true)
          .MR_ADD_MEMBER_PRIVATE(Scene, m_name, true);
      }
      public:
        Scene() = default;
        explicit Scene(bool active)
          : m_active(active) {}

        void AddGameObject(Handle<GameObject> gameObject);

        inline void SetSceneName(Container::String name) { m_name = name; }
        inline const Container::String& GetSceneName(void) const { return m_name; }

        inline const Container::Vector<SceneNode>& GetSceneNodes(void) const { return m_sceneNodes; }
        inline const Container::Vector<Handle<GameObject>>& GetAllGameObjects(void) const { return m_sceneGameObjects; }
      private:
        bool m_active = false;
        Container::String m_name;
        //TODO: Store set of Components to Update

        //Parallel Arrays of Scene Data
        Container::Vector<SceneNode> m_sceneNodes;
        Container::Vector<Handle<GameObject>> m_sceneGameObjects;
    };
  }
}