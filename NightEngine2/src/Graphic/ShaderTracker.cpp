/*!
  @file OpenglAllocationTracker.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of OpenglAllocationTracker
*/
#include "Graphic/ShaderTracker.hpp"
#include "Graphic/Opengl/Shader.hpp"

#include "Core/Logger.hpp"
#include "Core/Utility/Utility.hpp"

using namespace Core;

namespace Graphic
{
  // Store <ProgramID, Shader*>, so that we can recompile the Shader and assign the new ProgramID
  std::unordered_map<unsigned, std::unordered_set<Shader*>>& ShaderTracker::GetShaderMap()
  {
    static std::unordered_map<unsigned, std::unordered_set<Shader*>> map;
    return map;
  }

  void ShaderTracker::Add(Shader& shader)
  {
    auto& map = GetShaderMap();
    unsigned programID = shader.GetProgramID();

    map[programID].insert(&shader);
  }

  void ShaderTracker::Remove(Shader& shader)
  {
    auto& map = GetShaderMap();
    auto& set = map[shader.GetProgramID()];

    //Remove from the set inside Map
    auto it = set.find(&shader);
    if (it != set.end())
    {
      set.erase(it);
    }
    else
    {
      ASSERT_MSG(false, "Doesn't found shader reference inside set");
    }
  }

  void ShaderTracker::RecompileAllShaders()
  {
    Core::Utility::StopWatch stopWatch{ true };
    {
      std::vector<Shader*> shaders;

      //Recompile all Shaders
      auto& map = GetShaderMap();
      for (auto& pair : map)
      {
        auto& set = pair.second;
        for (auto shaderPtr : set)
        {
          shaders.emplace_back(shaderPtr);
        }
      }
      int i = 0;
      for (auto shaderPtr : shaders)
      {
        //if(i++ < 10)
        shaderPtr->RecompileShader();
      }
    }
    stopWatch.Stop();
    Debug::Log << Logger::MessageType::INFO
      << "RecompileAllShaders: [" << stopWatch.GetElapsedTimeMilli() << " ms]\n";
  }

  void ShaderTracker::Clear()
  {
    auto& map = GetShaderMap();
    map.clear();
  }
} // Graphic