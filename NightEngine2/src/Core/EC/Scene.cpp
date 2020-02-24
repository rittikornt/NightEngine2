/*!
  @file Scene.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Scene
*/
#include "Core/EC/Scene.hpp"

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"
#include "Graphics/Opengl/Light.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Reflection/Variable.hpp"

#include "Core/Serialization/FileSystem.hpp"

#include "Graphics/Opengl/InstanceDrawer.hpp"

using namespace Rendering;
using namespace NightEngine::EC;
using namespace NightEngine::EC::Components;

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

    namespace SceneManager
    {
      JsonValue SerializeScene(Reflection::Variable& variable)
      {
        using namespace NightEngine::Reflection;
        JsonValue value;
        auto& scene = variable.GetValue<Scene>();

        //Name
        Variable nameVar{ METATYPE_FROM_OBJECT(scene.m_name), &(scene.m_name) };
        value.emplace("m_name", nameVar.Serialize());

        //GameObjects
        if (scene.m_sceneGameObjects.size() > 0)
        {
          ValueObject gameObjectsListValue;
          for (auto g : scene.m_sceneGameObjects)
          {
            GameObject& go = *(g.Get());
            Variable goVar{ METATYPE_FROM_OBJECT(go), &(go) };

            gameObjectsListValue.append({ goVar.Serialize() });
          }
          value.emplace("m_sceneGameObjects", gameObjectsListValue);
        }

        //SceneNode informations
        if (scene.m_sceneNodes.size() > 0)
        {
          ValueObject sceneNodeListValue;
          for (auto& node : scene.m_sceneNodes)
          {
            Variable nodeVar{ METATYPE_FROM_OBJECT(node), &(node) };

            sceneNodeListValue.append({ nodeVar.Serialize() });
          }
          value.emplace("m_sceneNodes", sceneNodeListValue);
        }

        return value;
      }

      void DeserializeScene(ValueObject& valueObject, Reflection::Variable & variable)
      {
        using namespace NightEngine::Reflection;
        auto& scene = variable.GetValue<Scene>();

        //Find ValueObject Corresponding to member name
        auto& obj = valueObject.get_object();
        auto it = obj.find("m_name");
        if (it != obj.end())
        {
          scene.m_name = it->second.as<Container::String>();
        }
        else
        {
          Debug::Log << Logger::MessageType::ERROR_MSG
            << "Not Found Deserialize MemberName: m_name\n";
          ASSERT_TRUE(false);
        }

        //GameObjects, Creating each GameObject
        it = obj.find("m_sceneGameObjects");
        if (it != obj.end())
        {
          auto goArray = it->second.get_array();
          for (auto& go : goArray)
          {
            //New GameObject
            auto newGOHandle = GameObject::Create("Unname", 1);
            auto newGO = newGOHandle.Get();

            //Deserialize data
            Variable newGOVar{ METATYPE_FROM_OBJECT(*newGO), newGO };
            newGOVar.Deserialize(go);

            scene.m_sceneGameObjects.emplace_back(newGOHandle);
          }
        }
        else
        {
          Debug::Log << Logger::MessageType::WARNING
            << "Not Found Deserialize MemberName: m_sceneGameObjects\n";
        }

        //SceneNode informations
        it = obj.find("m_sceneNodes");
        if (it != obj.end())
        {
          auto sceneNodeArray = it->second.get_array();
          for (auto& node : sceneNodeArray)
          {
            SceneNode sceneNode;
            Variable sceneNodeVar{ METATYPE_FROM_OBJECT(sceneNode), &sceneNode };
            sceneNodeVar.Deserialize(node);

            scene.m_sceneNodes.emplace_back(sceneNode);
          }
        }
        else
        {
          Debug::Log << Logger::MessageType::WARNING
            << "Not Found Deserialize MemberName: m_sceneNodes\n";
        }
      }
    }
  }
}