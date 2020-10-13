/*!
  @file RenderLoop.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of IRenderLoop
*/
#include "Graphics/IRenderLoop.hpp";

namespace NightEngine::Rendering
{
  const ShadowsResolution k_shadowResEnum[4] = { ShadowsResolution::_512
    , ShadowsResolution::_1024, ShadowsResolution::_2048
    , ShadowsResolution::_4096};

  const char* k_debugViewStr[] = {"NONE", "ALBEDO","NORMAL_WORLD_SPACE", "METALLIC","ROUGHNESS", "EMISSIVE","LIGHT_SPACE_POS", "AMBIENT", "MAIN_SHADOW_DEPTH", "MOTION_VECTOR"};
  const char* k_debugShadowViewStr[] = { "NONE", "SHADOW_MAIN_ONLY", "SHADOW_ALL","SHADOW_CASCADE" };
} // Rendering
