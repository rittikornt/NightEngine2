/*!
  @file ArchetypeManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ArchetypeManager
*/
#pragma once

#include "Core/Logger.hpp"

#include "Core/Serialization/Serialization.hpp"

#include "Core/Container/Map.hpp"
#include "Core/Container/String.hpp"

using namespace Core::Serialization;

namespace Core
{
  namespace ECS
  {
    class GameObject;
    struct Archetype;

    namespace ArchetypeManager
    {
      //! @brief List of Archetype
      struct ArchetypeList
      {
        using ArchetypeMap = Container::Map<Container::String, Archetype>;
        ArchetypeMap m_archetypes;
      };

      //! @brief Archetype Serialize function
      JsonValue SerializeArchetypeList(Reflection::Variable& variable);

      //! @brief Archetype Deserialize function
      void DeserializeArchetypeList(ValueObject& valueObject, Reflection::Variable& variable);

      ///////////////////////////////////////////////////////////

      //! @brief Initialize ArchetypeManger
      void Initialize(void);

      //! @brief Terminate ArchetypeManger
      void Terminate(void);

      ///////////////////////////////////////////////////////////

      //! @brief Save to Blueprint
      void SaveToBlueprint(const std::string& name
        , Core::ECS::GameObject& gameObject);

      //! @brief Load Blueprint
      void LoadBlueprint(const std::string& name
        , Core::ECS::GameObject& gameObject);

      ///////////////////////////////////////////////////////////

      //! @brief Get the Archetype Map
      ArchetypeList::ArchetypeMap& GetArchetypeMap(void);

      //! @brief Reload ArchetypeList from File
      void RefreshArchetypeList(void);

      //! @brief Save to Archetype
      void SaveToArchetype(const std::string& name
        , Core::ECS::GameObject& gameObject);

      //! @brief Save to Archetype
      void SaveToArchetype(const std::string& name
        , Core::ECS::Archetype& archetype);

      //! @brief Load Archetype
      void LoadArchetype(const std::string& name
        , Core::ECS::GameObject& gameObject);

      //! @brief Remove from Archetype
      void RemoveFromArchetype(std::string name);

      //! @brief Remove from Archetype
      void RenameArchetype(const std::string& name
        ,const std::string& newName);

      //! @brief Check if gameObject is a specific Archetype
      bool IsArchetype(Core::ECS::GameObject& gameObject
        ,Archetype& archetype);
    }
  }
}