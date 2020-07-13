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

    float rVal = (IF_EXIST(texMap, ROUGHNESS_TEXUNIT_INDEX)) ? 1.0f: 0.5f;
    FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_roughnessValue, rVal)

    float mVal = (IF_EXIST(texMap, METALLIC_TEXUNIT_INDEX)) ? 1.0f : 0.1f;
    FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_metallicValue, mVal)

    FILL_IF_NOT_EXIST(floatMap, MP_PBRMetallic::m_emissiveStrength, 15.0f)
    FILL_IF_NOT_EXIST(vec4Map, MP_PBRMetallic::u_diffuseColor, glm::vec4(1.0f))

    FILL_IF_NOT_EXIST(texMap, DIFFUSE_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, NORMAL_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
    FILL_IF_NOT_EXIST(texMap, ROUGHNESS_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, METALLIC_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, EMISSIVE_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
  }

  void MP_PBRMetallic::RefreshTextureUniforms(const Shader& shader) const
  {
    shader.SetUniform(MP_PBRMetallic::m_diffuseMap, DIFFUSE_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_normalMap, NORMAL_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_roughnessMap, ROUGHNESS_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_metallicMap, METALLIC_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRMetallic::m_emissiveMap, EMISSIVE_TEXUNIT_INDEX);
  }

  ////////////////////////////////////////////////////////////

  const char* MP_PBRSpecularBumpmap::k_textureNames[5] =
  { "DiffuseMap" , "BumpMap", "SpecularMap", "MetallicMap", "EmissiveMap" };

  const char* MP_PBRSpecularBumpmap::m_diffuseMap = "u_material.m_diffuseMap";
  const char* MP_PBRSpecularBumpmap::u_diffuseColor = "u_diffuseColor";

  const char* MP_PBRSpecularBumpmap::u_useBumpmap = "u_useBumpmap";
  const char* MP_PBRSpecularBumpmap::m_bumpMultiplier = "u_material.m_bumpMultiplier";
  const char* MP_PBRSpecularBumpmap::m_bumpMap = "u_material.m_bumpMap";

  const char* MP_PBRSpecularBumpmap::m_specularMap = "u_material.m_specularMap";
  const char* MP_PBRSpecularBumpmap::m_specularMul = "u_material.m_specularMul";

  const char* MP_PBRSpecularBumpmap::m_metallicMap = "u_material.m_metallicMap";
  const char* MP_PBRSpecularBumpmap::m_metallicValue = "u_material.m_metallicValue";

  const char* MP_PBRSpecularBumpmap::m_emissiveMap = "u_material.m_emissiveMap";
  const char* MP_PBRSpecularBumpmap::m_emissiveStrength = "u_material.m_emissiveStrength";

  static const std::unordered_map<std::string, IMGUIEditorData> s_SBMap
  {
    {"u_diffuseColor", {IMGUIEditorType::COLOR4}}
  , {"u_useBumpmap", {IMGUIEditorType::CHECKBOX}}
  , {"u_useMetallicMap", {IMGUIEditorType::CHECKBOX}}
  , {"u_material.m_bumpMultiplier", {IMGUIEditorType::DRAGSCALAR, 0.01f, 5.0f}}
  , {"u_material.m_specularMul", {IMGUIEditorType::DRAGSCALAR, 0.0f, 1.0f}}
  , {"u_material.m_metallicValue", {IMGUIEditorType::DRAGSCALAR, 0.0f, 1.0f}}
  , {"u_material.m_emissiveStrength", {IMGUIEditorType::DRAGSCALAR, 0.0f, 20.0f}}
  };

  void MP_PBRSpecularBumpmap::Init(Material& material) const
  {
    material.SetMaterialProperty(this);

    auto& texMap = material.GetTextureMap();
    auto& vec4Map = material.GetVec4Map();
    auto& floatMap = material.GetFloatMap();
    auto& intMap = material.GetIntMap();

    FILL_IF_NOT_EXIST(intMap, MP_PBRSpecularBumpmap::u_useBumpmap, false)
    FILL_IF_NOT_EXIST(floatMap, MP_PBRSpecularBumpmap::m_bumpMultiplier, 1.0f)

    float sVal = (IF_EXIST(texMap, SPECULAR_TEXUNIT_INDEX)) ? 1.0f : 0.5f;
    FILL_IF_NOT_EXIST(floatMap, MP_PBRSpecularBumpmap::m_specularMul, sVal)

    float mVal = (IF_EXIST(texMap, METALLIC_TEXUNIT_INDEX)) ? 1.0f : 0.1f;
    FILL_IF_NOT_EXIST(floatMap, MP_PBRSpecularBumpmap::m_metallicValue, mVal)

    FILL_IF_NOT_EXIST(floatMap, MP_PBRSpecularBumpmap::m_emissiveStrength, 15.0f)
    FILL_IF_NOT_EXIST(vec4Map, MP_PBRSpecularBumpmap::u_diffuseColor, glm::vec4(1.0f))

    FILL_IF_NOT_EXIST(texMap, DIFFUSE_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, BUMP_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
    FILL_IF_NOT_EXIST(texMap, SPECULAR_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, METALLIC_TEXUNIT_INDEX, ResourceManager::GetWhiteTexture())
    FILL_IF_NOT_EXIST(texMap, EMISSIVE_TEXUNIT_INDEX, ResourceManager::GetBlackTexture())
  }

  void MP_PBRSpecularBumpmap::RefreshTextureUniforms(const Shader& shader) const
  {
    shader.SetUniform(MP_PBRSpecularBumpmap::m_diffuseMap, DIFFUSE_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRSpecularBumpmap::m_bumpMap, BUMP_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRSpecularBumpmap::m_specularMap, SPECULAR_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRSpecularBumpmap::m_metallicMap, METALLIC_TEXUNIT_INDEX);
    shader.SetUniform(MP_PBRSpecularBumpmap::m_emissiveMap, EMISSIVE_TEXUNIT_INDEX);
  }

  NightEngine::IMGUI::IMGUIEditorData MP_PBRSpecularBumpmap::GetEditorData(const char* name) const
  {
    auto it = s_SBMap.find(name);
    if (it != s_SBMap.end())
    {
      return it->second;
    }

    return IMGUIEditorData{ IMGUIEditorType::DEFAULT };
  }
}