/*!
  @file OpenglAllocationTracker.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of OpenglAllocationTracker
*/
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"
#include "Core/Logger.hpp"

#include <unordered_map>
#include <string>

using namespace Core;

namespace Graphic
{
  static std::unordered_map<std::string, std::unordered_set<unsigned int>> s_allocationTracker;

  void OpenglAllocationTracker::Increment(const char * typeName, unsigned int id)
  {
    s_allocationTracker[typeName].insert(id);

    Debug::Log << Logger::MessageType::INFO
      << typeName << " Alloc: " << id << '\n';
  }

  void OpenglAllocationTracker::Decrement(const char * typeName, unsigned int id)
  {
    auto it = s_allocationTracker.find(typeName);
    if (it != s_allocationTracker.end())
    {
      (it->second).erase(id);
      //ASSERT_MSG(it->second >= 0, "Trying to Decrement allocation count that doesn't exist");
    }
    else
    {
      ASSERT_MSG(false, "Trying to Decrement allocation count that doesn't exist");
    }

    Debug::Log << Logger::MessageType::INFO
      << typeName << " Release: " << id << '\n';
  }

  bool OpenglAllocationTracker::Exist(const char * typeName, unsigned int id)
  {
    auto it = s_allocationTracker.find(typeName);

    //Can't exist if the set for this typeName is never initialized
    if (it == s_allocationTracker.end())
    {
      return false;
    }

    //Check if the id exist
    auto it2 = it->second.find(id);
    return it2 != it->second.end()? true: false;
  }

  std::unordered_set<unsigned int>& OpenglAllocationTracker::GetAllocatedObjectID(const char* typeName)
  {
    auto it = s_allocationTracker.find(typeName);
    return it->second;
  }

  void OpenglAllocationTracker::DeallocateAllObjects(const char * typeName, DeallcateFunc func)
  {
    auto it = s_allocationTracker.find(typeName);
    if (it != s_allocationTracker.end())
    {
      Debug::Log << "-----------------------------------------\n";
      Debug::Log << Logger::MessageType::INFO
        << typeName << " Size: " << it->second.size() << '\n';

      //Since the func is expected to call OpenglAllocationTracker::Decrement
      //which will modify the set, looping through set calling func will throw access violation
      //due to set being modified while looping through it
      std::vector<unsigned int> idToDealloc;
      idToDealloc.reserve(it->second.size());

      //Traverse all Object id
      for (auto id : it->second)
      {
        idToDealloc.emplace_back(id);
      }

      //defer dealloc
      for (auto id : idToDealloc)
      {
        func(id);
      }

      Debug::Log << Logger::MessageType::INFO
        << typeName << " Size: " << it->second.size() << '\n';
      Debug::Log << "-----------------------------------------\n";
    }
    else
    {
      //ASSERT_MSG(false, "Trying to deallocate an empty set");
    }
  }

  void OpenglAllocationTracker::PrintAllocationState()
  {
    Debug::Log << "*****************************************************\n";
    Debug::Log << "OpenglAllocationTracker: OpenglAllocationTracker\n";
    for (auto& pair : s_allocationTracker)
    {
      Debug::Log << "-" << pair.first << ": " << pair.second.size() << "\n";
    }
    Debug::Log << "*****************************************************\n";
  }
} // Graphic