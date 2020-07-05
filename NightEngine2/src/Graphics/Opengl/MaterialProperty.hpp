/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MaterialProperty
*/
#pragma once

#define DIFFUSE_TEXUNIT_INDEX 0
#define NORMAL_TEXUNIT_INDEX 1
#define ROUGHNESS_TEXUNIT_INDEX 2
#define METALLIC_TEXUNIT_INDEX 3
#define EMISSIVE_TEXUNIT_INDEX 4

namespace Rendering
{
  class Shader;
}

namespace Rendering
{
  class MaterialProperty
  {
  public:
    template<typename T>
    static void SetTextureBindingUnit(const Shader& shader)
    {
      T::SetTextureBindingUnit(shader);
    }
  };

  struct MP_PBRMetallic
  {
    static const char* k_textureNames[5];

    static const char* m_diffuseMap;
    static const char* u_diffuseColor;

    static const char* u_useNormalmap;
    static const char* m_normalMultiplier;
    static const char* m_normalMap;

    static const char* m_roughnessMap;
    static const char* m_roughnessValue;

    static const char* m_metallicMap;
    static const char* m_metallicValue;

    static const char* m_emissiveMap;
    static const char* m_emissiveStrength;

    static void SetTextureBindingUnit(const Shader& shader);
  };
}