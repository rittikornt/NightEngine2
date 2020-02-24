/*!
  @file SceneNode.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of SceneNode
*/

#include "Core/EC/SceneNode.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Reflection/Variable.hpp"

using namespace NightEngine::EC;

namespace NightEngine
{
  namespace EC
  {
    INIT_REFLECTION_FOR(SceneNode)

    namespace SceneManager
    {
      JsonValue SerializeSceneNode(Reflection::Variable& variable)
      {
        using namespace NightEngine::Reflection;
        JsonValue value;
        auto& sceneNode = variable.GetValue<SceneNode>();

        //Parent Index
        Variable pIndexVar{ METATYPE_FROM_OBJECT(sceneNode.m_parentIndex), &(sceneNode.m_parentIndex) };
        value.emplace("m_parentIndex", pIndexVar.Serialize());

        //Children Index
        if (sceneNode.m_children.size() > 0)
        {
          ValueObject childrenIndexListValue;
          for (auto childIndex : sceneNode.m_children)
          {
            Variable ciVar{ METATYPE_FROM_OBJECT(childIndex), &(childIndex) };

            childrenIndexListValue.append({ ciVar.Serialize() });
          }
          value.emplace("m_children", childrenIndexListValue);
        }

        return value;
      }

      void DeserializeSceneNode(ValueObject& valueObject, Reflection::Variable& variable)
      {
        using namespace NightEngine::Reflection;

        auto& sceneNode = variable.GetValue<SceneNode>();

        //Find ValueObject Corresponding to member name
        auto& obj = valueObject.get_object();
        auto it = obj.find("m_parentIndex");
        if (it != obj.end())
        {
          sceneNode.m_parentIndex = it->second.as<int>();
        }
        else
        {
          Debug::Log << Logger::MessageType::ERROR_MSG
            << "Not Found Deserialize MemberName: m_parentIndex\n";
          ASSERT_TRUE(false);
        }

        //Children Index
        it = obj.find("m_children");
        if (it != obj.end())
        {
          sceneNode.m_children.clear();

          auto array = it->second.get_array();
          for (auto& elem : array)
          {
            sceneNode.m_children.insert({ elem.as<int>() });
          }
        }
        else
        {
          Debug::Log << Logger::MessageType::WARNING
            << "Not Found Deserialize MemberName: m_children\n";
        }
      }
    }
  }
}