/*!
  @file OpenglAllocationTracker.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of OpenglAllocationTracker
*/
#pragma once
#include <unordered_set>
#include <unordered_map>

namespace NightEngine::Rendering::Opengl
{
  class Shader;

  struct ShaderTracker
  {
    // Store <ProgramID, Shader*>, so that we can recompile the Shader and assign the new ProgramID
    static std::unordered_map<unsigned, std::unordered_set<Shader*>>& GetShaderMap();

    static void Add(Shader& shader);

    static void Remove(Shader& shader);

    static void RecompileAllShaders();

    static void Clear();
  };
} // Rendering

