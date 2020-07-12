/*!
  @file RenderLoop.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of IRenderLoop
*/
#include "Graphics/IRenderLoop.hpp";

namespace Rendering
{
  const char* k_debugViewStr[] = {"NONE", "ALBEDO","NORMAL_WORLD_SPACE", "METALLIC","ROUGHNESS", "EMISSIVE","LIGHT_SPACE_POS", "AMBIENT"};
  const char* k_debugShadowViewStr[] = { "NONE", "SHADOW_ONLY","SHADOW_CASCADE" };
} // Rendering
