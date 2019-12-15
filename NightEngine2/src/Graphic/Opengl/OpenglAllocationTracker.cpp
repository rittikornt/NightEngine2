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
  static std::unordered_map<std::string, int> s_allocationTracker;

  void OpenglAllocationTracker::Increment(const char * typeName)
  {
    auto it = s_allocationTracker.find(typeName);
    if (it != s_allocationTracker.end())
    {
      ++(it->second);
    }
    else
    {
      s_allocationTracker[typeName] = 1;
    }
  }

  void OpenglAllocationTracker::Decrement(const char * typeName)
  {
    auto it = s_allocationTracker.find(typeName);
    if (it != s_allocationTracker.end())
    {
      --(it->second);
      //ASSERT_MSG(it->second >= 0, "Trying to Decrement allocation count that doesn't exist");
    }
    else
    {
      ASSERT_MSG(false, "Trying to Decrement allocation count that doesn't exist");
    }
  }

  void OpenglAllocationTracker::PrintAllocationState()
  {
    Debug::Log << "OpenglAllocationTracker: OpenglAllocationTracker\n";
    for (auto& pair : s_allocationTracker)
    {
      Debug::Log << "-" << pair.first << ": " << pair.second << "\n";
    }
  }
} // Graphic