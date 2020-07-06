/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Header of MaterialProperty
*/
#pragma once

#define DEFAULT_VERTEX_SHADER_PBR "Rendering/deferred_geometry_pass.vert"
#define DEFAULT_FRAG_SHADER_PBR "Rendering/deferred_geometry_pass.frag"

#define DIFFUSE_TEXUNIT_INDEX 0
#define NORMAL_TEXUNIT_INDEX 1
#define ROUGHNESS_TEXUNIT_INDEX 2
#define METALLIC_TEXUNIT_INDEX 3
#define EMISSIVE_TEXUNIT_INDEX 4

namespace NightEngine
{
  namespace IMGUI
  {
    enum class IMGUIEditorType : unsigned
    {
      DEFAULT = 0,
      CHECKBOX,
      INPUTSCALAR,
      DRAGSCALAR,
      SLIDER,
      COLOR4, //Vec4
    };

    struct IMGUIEditorData
    {
      IMGUIEditorType type = IMGUIEditorType::DEFAULT;
      float min = 0.0f;
      float max = 1.0f;
      float speed = 0.05f;
    };
  }
}

namespace Rendering
{
  class Shader;

  struct MaterialProperty
  {
    template<typename T>
    static const T& Get(void)
    {
      static T mp;
      return mp;
    }

    virtual NightEngine::IMGUI::IMGUIEditorData GetEditorData(const char* name) const = 0;
  };

  struct MP_PBRMetallic: public MaterialProperty
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

    NightEngine::IMGUI::IMGUIEditorData GetEditorData(const char* name) const override;

    static void SetTextureBindingUnit(const Shader& shader);
  };

  ////////////////////////////////////////////////////////////

  //TODO: Fill this for sponza
  struct MP_PBRSpecularBumpmap
  {
    static const char* k_textureNames[5];

    static const char* m_diffuseMap;
    static const char* u_diffuseColor;

    static const char* u_useBumpmap;
    static const char* m_bumpMultiplier;
    static const char* m_bumpMap;

    static const char* m_specularMap;
    static const char* m_specularValue;

    static const char* m_metallicMap;
    static const char* m_metallicValue;

    static const char* m_emissiveMap;
    static const char* m_emissiveStrength;

    static void SetTextureBindingUnit(const Shader& shader);
  };
}