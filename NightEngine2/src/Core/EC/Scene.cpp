/*!
  @file Scene.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Scene
*/
#include "Core/EC/Scene.hpp"

namespace NightEngine
{
  namespace EC
  {
    INIT_REFLECTION_FOR(Scene)

    void Scene::AddGameObject(Handle<GameObject> gameObject)
    {
      m_sceneGameObjects.emplace_back(gameObject);
      m_sceneNodes.emplace_back(SceneNode());
    }
  }
}