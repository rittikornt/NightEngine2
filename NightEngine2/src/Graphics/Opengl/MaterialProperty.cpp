/*!
  @file Material.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MaterialProperty
*/

#include "Graphics/Opengl/MaterialProperty.hpp"

#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Material.hpp"

#include "Core/Serialization/ResourceManager.hpp"

#include <unordered_map>

using namespace NightEngine;
using namespace NightEngine::IMGUI;

#define FILL_IF_NOT_EXIST(TABLE, PROPERTY, VAL) \
        if(TABLE.find(PROPERTY) == TABLE.end()) \
          { TABLE[PROPERTY] = VAL; }

#define IF_EXIST(TABLE, PROPERTY) \
        (TABLE.find(PROPERTY) != TABLE.end())

namespace NightEngine::Rendering::Opengl
{
  const char* MP_PBRMetallic::k_textureNames[6] =
  { "DiffuseMap" , "NormalMap", "RoughnessMap", "MetallicMap", "EmissiveMap", "OpacityMap" };

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

  const char* MP_PBRMetallic::u_useOpacityMap = "u_useOpacityMap";
  const char* MP_PBRMetallic::m_opacityMap = "u_material.m_opacityMap";
  const char* MP_PBRMetallic::m_cutOffValue = "u_material.m_cutOffValue";

  static const std::unordered_map<std::string, IMGUIEditorData> s_PBRMetallicMap
  {
    {"u_diffuseColor", {IMGUIEditorType::COLOR4}}
  , {"u_useNormalmap", {IMGUIEditorType::CHECKBOX}}
  , {"u_useRoughnessMap", {IMGUIEditorType::CHECKBOX}}
  , {"u_useMetallicMap", {IMGUIEditorType::CHECKBOX}}
  , {"u_material.m_normalMultiplier", {IMGUIEditorType::DRAGSCALAR, 0.01f, 5.0f}}
  , {"u_material.m_roughnessValue", {IMGUIEditorType::DRAGSCALAR, 0.0f, 1.0f}}
  , {"u_material.m_metallicValue", {IMGUIEditorType::DRAGSCALAR, 0.0f, 1.0f}}
  , {"u_material.m_emissiveStrength", {IMGUIEditorType::DRAGSCALAR, 0.0f, 20.0f}}
  , {"u_useOpacityMap", {IMGUIEditorType::CHECKBOX}}
  , {"u_material.m_cutOffValue", {IMGUIEditorType::DRAGSCALAR, 0.0f, 1.0f}}
  };

  NightEngine::IMGUI::IMGUIEditorData MP_PBRMetallic::GetEditorData(const char* name) const
  {
    auto it = s_PBRMetallicMap.find(name);
    if (it != s_PBRMetallicMap.end())
    {
      return it->second;
    }

    return IMGUIEditorData{ IMGUIEditorType::DEFAULT };
  }

  void MP_PBRMetallic::Init(Material& material) const
  {
    material.SetMaterialProperty(this);

    auto& texMap = material.GetTextureMap();
    auto& vec4Map = material.GetVec4Map();
    auto& floatMap = material.GetFloatMap();
    auto& intMap = material.GetIntMap();

      FILL_IF_NOT_EXIST(intMap, MP_PBRMetallic::u_useNormalmap, false)
      FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_normalMultiplier, 1.0f)

      float rVal = (IF_EXIST(texMap, ROUGHNESS_TEXUNIT_INDEX)) ? 1.0f : 0.5f;
      FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_roughnessValue, rVal)

      float mVal = (IF_EXIST(texMap, METALLIC_TEXUNIT_INDEX)) ? 1.0f : 0.1f;
      FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_metallicValue, mVal)

      FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_emissiveStrength, 5.0f)
      FILL_IF_NOT_EXIST(vec4Map, MP_PBRMetallic::u_diffuseColor, glm::vec4(1.0f))

      FILL_IF_NOT_EXIST(intMap, MP_PBRMetallic::u_useOpacityMap, false)
      FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_cutOffValue, 0.9f)

      FILL_IF_NOT_EXIST(texMap, DIFFUSE_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
      FILL_IF_NOT_EXIST(texMap, NORMAL_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
      FILL_IF_NOT_EXIST(texMap, ROUGHNESS_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
      FILL_IF_NOT_EXIST(texMap, METALLIC_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
      FILL_IF_NOT_EXIST(texMap, EMISSIVE_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
      FILL_IF_NOT_EXIST(texMap, OPACITYMASK_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
  }

  void MP_PBRMetallic::RefreshTextureUniforms(const Shader& shader) const
  {
    shader.SetUniform(MP_PBRMetallic::m_diffuseMap, DIFFUSE_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_normalMap, NORMAL_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_roughnessMap, ROUGHNESS_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_metallicMap, METALLIC_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_emissiveMap, EMISSIVE_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_opacityMap, OPACITYMASK_TEXUNIT_INDEX);
  }
}