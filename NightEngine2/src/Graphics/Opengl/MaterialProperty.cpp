/*!
  @file Material.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MaterialProperty
*/

#include "Graphics/Opengl/MaterialProperty.hpp"
#include "Graphics/Opengl/Shader.hpp"

namespace Rendering
{
  const char* MP_PBRMetallic::k_textureNames[5] = 
  { "DiffuseMap" , "NormalMap", "RoughnessMap", "MetallicMap", "EmissiveMap" };

  const char* MP_PBRMetallic::m_diffuseMap = "u_material.m_diffuseMap";
  const char* MP_PBRMetallic::u_diffuseColor = "u_diffuseColor";

  const char* MP_PBRMetallic::u_useNormalmap = "u_useNormalmap";
  const char* MP_PBRMetallic::m_normalMultiplier = "u_material.m_normalMultiplier";
  const char* MP_PBRMetallic::m_normalMap = "u_material.m_normalMap";

  const char* MP_PBRMetallic::m_roughnessMap = "u_material.m_roughnessMap";
  const char* MP_PBRMetallic::m_roughnessValue = "u_material.m_roughnessValue";

  const char* MP_PBRMetallic::m_metallicMap = "u_material.m_metallicMap";
  const char* MP_PBRMetallic::m_metallicValue = "u_material.m_metallicValue";

  const char* MP_PBRMetallic::m_emissiveMap = "u_material.m_emissiveMap";
  const char* MP_PBRMetallic::m_emissiveStrength = "u_material.m_emissiveStrength";
  
  void MP_PBRMetallic::SetTextureBindingUnit(const Shader& shader)
  {
    shader.SetUniform(MP_PBRMetallic::m_diffuseMap, DIFFUSE_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_normalMap, NORMAL_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_roughnessMap, ROUGHNESS_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_metallicMap, METALLIC_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_emissiveMap, EMISSIVE_TEXUNIT_INDEX);
  }

  ////////////////////////////////////////////////////////////
}