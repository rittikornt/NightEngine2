/*!
  @file Archetype.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Archetype
*/

#include "Archetype.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

#include "Core/EC/GameObject.hpp"
#include "Core/Reflection/Variable.hpp"

#include "Core/Macros.hpp"

using namespace Core::Reflection;

namespace Core
{
  namespace ECS
  {
    Archetype::Archetype(const Container::String& name,GameObject& gameObject)
     : m_name(name)
    {
      //Get All gameObject's Component Types
      auto& components = gameObject.GetAllComponents();
      for (auto& c : components)
      {
        m_componentTypes.insert(c.m_metaType->GetName());
      }
    }

    void Archetype::AddComponent(const Container::String & componentName)
    {
      m_componentTypes.insert({ componentName });
    }

    void Archetype::RemoveComponent(const Container::String & componentName)
    {
      m_componentTypes.erase(componentName);
    }
    
    JsonValue SerializeArchetype(Reflection::Variable& variable)
    {
      JsonValue value;
      auto archetype = variable.GetValue<Archetype>();

      //Name
      Variable nameVar{ METATYPE_FROM_OBJECT(archetype.m_name), &(archetype.m_name) };
      value.emplace("m_name", nameVar.Serialize());

      //ComponentTypes
      if (archetype.m_componentTypes.size() > 0)
      {
        ValueObject componentTypesValue;
        for (auto& ct : archetype.m_componentTypes)
        {
          componentTypesValue.append({ ct });
        }
        value.emplace("m_componentTypes", componentTypesValue);
      }

      return value;
    }

    void DeserializeArchetype(ValueObject& valueObject, Reflection::Variable& variable)
    {
      auto& archetype = variable.GetValue<Archetype>();

      //Find ValueObject Corresponding to member name
      auto& obj = valueObject.get_object();
      auto it = obj.find("m_name");
      if (it != obj.end())
      {
        archetype.m_name = it->second.as<Container::String>();
      }
      else
      {
        Debug::Log << Logger::MessageType::ERROR_MSG
          << "Not Found Deserialize MemberName: m_name\n";
        ASSERT_TRUE(false);
      }

      //Component Types
      it = obj.find("m_componentTypes");
      if (it != obj.end())
      {
        auto array = it->second.get_array();
        for (auto& elem : array)
        {
          archetype.m_componentTypes.insert({ elem.as<Container::String>()});
        }
      }
      else
      {
        Debug::Log << Logger::MessageType::WARNING
          << "Not Found Deserialize MemberName: m_componentTypes\n";
      }
    }
  }

}

