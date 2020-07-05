/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of File
*/
#pragma once

#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/Shader.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

#include "Core/EC/Handle.hpp"
#include <unordered_map>

#define DEFAULT_VERTEX_SHADER_PBR "Rendering/deferred_geometry_pass.vert"
#define DEFAULT_FRAG_SHADER_PBR "Rendering/deferred_geometry_pass.frag"

#define TEXTURE_TABLE(TYPE) std::unordered_map<int, TYPE>
#define PROPERTY_TABLE(TYPE) std::unordered_map<std::string, TYPE>

namespace Rendering
{
  struct PBRShaderParams
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
  };
}

namespace Rendering
{
  class Material
  {
    REFLECTABLE_TYPE_BLOCK()
    {
      META_REGISTERER(Material, true
        , NightEngine::Serialization::DefaultSerializer<Material&>
        , NightEngine::Serialization::DefaultDeserializer<Material&>)
        .MR_ADD_MEMBER_PRIVATE(Material, m_name, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_filePath, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_diffuseTexture, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_diffuseColor, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_normalTexture, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_normalMultiplier, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_useNormal, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_roughnessTexture, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_roughnessValue, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_metallicTexture, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_metallicValue, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_emissiveTexture, true)
        //.MR_ADD_MEMBER_PRIVATE(Material, m_emissiveStrength, true)

        .MR_ADD_MEMBER_PRIVATE(Material, m_textureMap, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_vec4Map, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_floatMap, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_intMap, true);
    }

    public:
      //! @brief default constructor
      Material(void) = default;

      //! @brief default constructor
      Material(const Material& rhs);

      //! @brief Assignment Operator
      Material& operator=(const Material& rhs);

      //! @brief Initialize Shader
		  void	InitShader(const std::string& vertexShader
      , const std::string& fragmentShader);

      //! @brief Initialize Shader
      void	InitShader(const std::string& vertexShader
        , const std::string& fragmentShader, const std::string& geometryShader);

      //! @brief Initialize Texture
      void	InitTexture(const std::string& diffuseTextureFile
        , bool useNormal
        , const std::string& normalTextureFile
        , const std::string& roughnessTextureFile
        , const std::string& metallicTextureFile
        , const std::string& emissiveTextureFile);

      //! @brief Initialize Texture
      void RefreshTextureUniforms(void) const;

      //! @brief Apply material to the shader
      void Bind(bool useTexture = true);

      //! @brief Unbind the Shader
      void Unbind(void);

      //! @brief Clear Material
      void Clear(void);

      //! @brief Load Material from File (eg. "fileName.mat")
      static NightEngine::EC::Handle<Material> LoadMaterial(const std::string& fileName);

      //! @brief Save Material from File
      static void SaveMaterial(const std::string& fileName, Material& material);

      //! @brief Preload All Materials in the Path
      static void PreLoadAllMaterials(void);

      //////////////////////////////////////////////////////////////

      //! @brief Get Shader
      Shader& GetShader(void) { return m_shader; }

      //! @brief Get Name
      inline const std::string& GetName(void) const { return m_name; }

      //! @brief Get Load File Path
      inline const std::string& GetFilePath(void) const { return m_filePath; }

      //! @brief Set the material params
      void SetParams(float roughness, float metallic);

      //! @brief Set the material name
      inline void SetName(std::string name) { m_name = name; }

      //! @brief Set Load File Path
      inline void SetFilePath(std::string path) { m_filePath = path; }

    private:
      std::string m_name = "unnamed";
      std::string m_filePath = "";
      Shader      m_shader;

      TEXTURE_TABLE(NightEngine::EC::Handle<Texture>) m_textureMap;
      PROPERTY_TABLE(glm::vec4) m_vec4Map;
      PROPERTY_TABLE(float)     m_floatMap;
      PROPERTY_TABLE(int)       m_intMap;

      //NightEngine::EC::Handle<Texture> m_diffuseTexture;
      //glm::vec3   m_diffuseColor = glm::vec3(1.0f);
      //
      //NightEngine::EC::Handle<Texture>  m_normalTexture;
      //float       m_normalMultiplier  = 1.0f;
      //bool        m_useNormal = false;
      //
      //NightEngine::EC::Handle<Texture>  m_roughnessTexture;
      //float 	    m_roughnessValue    = 0.01f;
      //
      //NightEngine::EC::Handle<Texture>  m_metallicTexture;
      //float 	    m_metallicValue     = 0.1f;
      //
      //NightEngine::EC::Handle<Texture> m_emissiveTexture;
      //float 	    m_emissiveStrength = 15.0f;
  };
}