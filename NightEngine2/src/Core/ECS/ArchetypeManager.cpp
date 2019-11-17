/*!
  @file ArchetypeManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ArchetypeManager
*/

#include "ArchetypeManager.hpp"
#include "Core/ECS/Archetype.hpp"

#include "Core/ECS/GameObject.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

#include "Core/Reflection/Variable.hpp"

using namespace Core::Reflection;

namespace Core
{
  namespace ECS
  {
    namespace ArchetypeManager
    {
      ArchetypeList g_archetypeList;
      std::string   g_archetypeListFileName{ "ArchetypeList.archetypelist"};

      //TODO: Blueprint Track/Link System

      JsonValue SerializeArchetypeList(Reflection::Variable& variable)
      {
        JsonValue value;
        auto& archetypes = variable.GetValue<ArchetypeList::ArchetypeMap>();

        //Serialize each pair into JsonValue
        if(archetypes.size() > 0)
        {
          JsonValue archetypesValue;
          for (auto& it : archetypes)
          {
            Variable var{ METATYPE_FROM_OBJECT(it.second)
              , &(it.second) };
            archetypesValue.emplace(it.first, var.Serialize());
          }
          value.emplace("m_archetypes", archetypesValue);
        }

        return value;
      }

      void DeserializeArchetypeList(ValueObject & valueObject, Reflection::Variable& variable)
      {
        auto& archetypeList = variable.GetValue<ArchetypeList>();

        //Find ValueObject Corresponding to member name
        auto& obj = valueObject.get_object();
        auto it = obj.find("m_archetypes");
        if (it != obj.end())
        {
          //Archetypes <Name, Archetype> Map
          auto& map = it->second.get_object();
          for (auto& p : map)
          {
            //Deserialize Archetype value 
            Archetype archetype;
            Variable archetypeVar{ METATYPE_FROM_OBJECT(archetype)
              , &archetype };
            archetypeVar.Deserialize(p.second);

            //Add to list
            archetypeList.m_archetypes.insert( { p.first 
              , archetype } );
          }
        }
        else
        {
          Debug::Log << Logger::MessageType::ERROR_MSG
            << "Not Found Deserialize MemberName: m_archetypes\n";
          ASSERT_TRUE(false);
        }
      }

      ///////////////////////////////////////////////////////////

      void Initialize(void)
      {
        Debug::Log << "ArchetypeManager::Initialize";
        RefreshArchetypeList();
      }

      void Terminate(void)
      {
        Debug::Log << "ArchetypeManager::Terminate";
      }

      ///////////////////////////////////////////////////////////

      void SaveToBlueprint(const std::string& name, Core::ECS::GameObject & gameObject)
      {
        std::string fileName{ name };
        fileName += ".bp";
        Serialization::SerializeToFile(gameObject
          , fileName
          , FileSystem::DirectoryType::Archetypes);

        Debug::Log << Logger::MessageType::INFO
          << "Saved Blueprint:" << fileName << '\n';
      }

      void LoadBlueprint(const std::string & name, Core::ECS::GameObject& gameObject)
      {
        std::string fileName{ name };
        fileName += ".bp";
        Serialization::Deserialize(gameObject
          , fileName
          , FileSystem::DirectoryType::Archetypes);

        Debug::Log << Logger::MessageType::INFO 
          << "Load Blueprint:" << fileName << '\n';
      }

      ///////////////////////////////////////////////////////////

      ArchetypeList::ArchetypeMap & GetArchetypeMap(void)
      {
        return g_archetypeList.m_archetypes;
      }

      void RefreshArchetypeList(void)
      {
        g_archetypeList.m_archetypes.clear();

        //Load ArchetypeList Setting from file
        Serialization::Deserialize(g_archetypeList
          , g_archetypeListFileName
          , FileSystem::DirectoryType::Archetypes);
      }

      void SaveToArchetype(const std::string & name, Core::ECS::GameObject& gameObject)
      {
        //Add Archetype to ArchetypeList
        g_archetypeList.m_archetypes.insert({ name, Archetype(name,gameObject)});

        //Serialize it to ArchetypeList
        Serialization::SerializeToFile(g_archetypeList
          ,  g_archetypeListFileName
          , FileSystem::DirectoryType::Archetypes);

        Debug::Log << Logger::MessageType::INFO 
          << "Saved to ArchetypeList: " << name << '\n';
      }

      void SaveToArchetype(const std::string& name, Core::ECS::Archetype & archetype)
      {
        //Add Archetype to ArchetypeList
        archetype.m_name = name;
        g_archetypeList.m_archetypes.insert({ name, archetype });

        //Serialize it to ArchetypeList
        Serialization::SerializeToFile(g_archetypeList
          , g_archetypeListFileName
          , FileSystem::DirectoryType::Archetypes);

        Debug::Log << Logger::MessageType::INFO 
          << "Saved to ArchetypeList: " << name << '\n';
      }

      void LoadArchetype(const std::string& name, Core::ECS::GameObject & gameObject)
      {
        //Lookup from table
        auto it = g_archetypeList.m_archetypes.find(name);
        if (it != g_archetypeList.m_archetypes.end())
        {
          //Check if GameObject is already an Archetype
          if (IsArchetype(gameObject, it->second))
          {
            Debug::Log << Logger::MessageType::WARNING 
              << "GameObject is already an Archetype: " << name << '\n';
            return;
          }

          //Remove and add all components to match Archetype
          gameObject.RemoveAllComponents();
          for (auto cName: it->second.m_componentTypes)
          {
            gameObject.AddComponent(cName.c_str());
          }

          Debug::Log << Logger::MessageType::INFO 
            << "Loaded Archetype: " << name << '\n';
        }
        else
        {
          Debug::Log << Logger::MessageType::WARNING 
            << "Archetype Not Found: " << name << '\n';
        }
      }

      void RemoveFromArchetype(std::string name)
      {
        //Add Archetype to ArchetypeList
        auto it = g_archetypeList.m_archetypes.find(name);
        if (it != g_archetypeList.m_archetypes.end())
        {
          g_archetypeList.m_archetypes.erase(it);
        }

        //Serialize it to ArchetypeList
        Serialization::SerializeToFile(g_archetypeList
          , g_archetypeListFileName
          , FileSystem::DirectoryType::Archetypes);

        Debug::Log << Logger::MessageType::INFO 
          << "Removed from ArchetypeList: " << name << '\n';
      }

      void RenameArchetype(const std::string& name, const std::string& newName)
      {
        //Add Archetype to ArchetypeList
        auto it = g_archetypeList.m_archetypes.find(name);
        if (it != g_archetypeList.m_archetypes.end())
        {
          Archetype temp = it->second;
          g_archetypeList.m_archetypes.erase(it);

          SaveToArchetype(newName, temp);
          Debug::Log << Logger::MessageType::INFO 
            << "Renamed Succesfully\n";
        }
        else
        {
          Debug::Log << Logger::MessageType::WARNING 
            << "Renamed Failed\n";
        }
      }

      bool IsArchetype(Core::ECS::GameObject & gameObject
        , Archetype& archetype)
      {
        auto& components = gameObject.GetAllComponents();

        //Not identical, if not the same size
        if (components.size() != archetype.m_componentTypes.size())
        {
          return false;
        }

        //Check identical
        for (auto& c : components)
        {
          //If not found
          auto it = archetype.m_componentTypes.find(c.m_metaType->GetName());
          if (it == archetype.m_componentTypes.end())
          {
            return false;
          }
        }

        return true;
      }
    }
  }
}