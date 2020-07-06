/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Header of MaterialProperty
*/
#pragma once
#include "Core/Reflection/RemoveQualifier.hpp"

#define DEFAULT_VERTEX_SHADER_PBR "Rendering/deferred_geometry_pass.vert"
#define DEFAULT_FRAG_SHADER_PBR "Rendering/deferred_geometry_pass.frag"

#define DEFAULT_VERTEX_SHADER_PBR_SB "Rendering/deferred_geometry_pass_specular_bump.vert"
#define DEFAULT_FRAG_SHADER_PBR_SB "Rendering/deferred_geometry_pass_specular_bump.frag"

#define DIFFUSE_TEXUNIT_INDEX 0
#define BUMP_TEXUNIT_INDEX 1
#define NORMAL_TEXUNIT_INDEX 1
#define SPECULAR_TEXUNIT_INDEX 2
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
  class Material;

  struct MaterialProperty
  {
    template<typename T>
    static const T& Get(void)
    {
      static RawType<T> mp;
      return mp;
    }

    virtual void Init(Material& material) const = 0;

    virtual void RefreshTextureUniforms(const Shader& shader) const = 0;

    virtual const char* GetName(int index) const = 0;

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

    void Init(Material& material) const override;

    void RefreshTextureUniforms(const Shader& shader) const override;

    const char* GetName(int index) const override { return k_textureNames[index]; }

    NightEngine::IMGUI::IMGUIEditorData GetEditorData(const char* name) const override;
  };

  ////////////////////////////////////////////////////////////

  struct MP_PBRSpecularBumpmap : public MaterialProperty
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

    void Init(Material& material) const override;

    void RefreshTextureUniforms(const Shader& shader) const override;

    const char* GetName(int index) const override { return k_textureNames[index]; }

    NightEngine::IMGUI::IMGUIEditorData GetEditorData(const char* name) const override;
  };
}