/*!
  @file Archetype.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Archetype
*/

#pragma once

#include "Core/Reflection/MetaType.hpp" //JsonValue

#include "Core/Container/String.hpp"
#include <unordered_set>

namespace NightEngine
{
  //Forward declaration
  namespace Reflection
  {
    class Variable;
  }
  namespace EC
  {
    class GameObject;
  }

  namespace EC
  {
    //! @brief Class describing a set of components
    struct Archetype
    {
      Container::String m_name;
      std::unordered_set<Container::String> m_componentTypes; 

      //! @brief Default Constructor
      Archetype(void) = default;

      //! @brief Constructor
      Archetype(const Container::String& name, GameObject& gameObject);

      //! @brief Add Component
      void AddComponent(const Container::String& componentName);

      //! @brief Remove Component
      void RemoveComponent(const Container::String& componentName);
    };

    //! @brief Archetype Serialize function
    JsonValue SerializeArchetype(Reflection::Variable& variable);

    //! @brief Archetype Deserialize function
    void DeserializeArchetype(ValueObject& valueObject,Reflection::Variable& variable);
  }
}