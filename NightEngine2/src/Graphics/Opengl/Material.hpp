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
        .MR_ADD_MEMBER_PRIVATE(Material, m_diffuseTexture, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_diffuseColor, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_normalTexture, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_normalMultiplier, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_useNormal, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_roughnessTexture, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_roughnessValue, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_metallicTexture, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_metallicValue, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_emissiveTexture, true)
        .MR_ADD_MEMBER_PRIVATE(Material, m_emissiveStrength, true);
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
        , const std::string& metallicTexture
        , const std::string& emissiveTextureFile);

      //! @brief Initialize Texture
      void RefreshTextureUniforms(void) const;

      //! @brief Get Shader
      Shader& GetShader(void) { return m_shader; }

      //! @brief Get Name
      inline const std::string& GetName(void) const { return m_name; }

      //! @brief Apply material to the shader
      void Bind(bool useTexture = true);

      //! @brief Unbind the Shader
      void Unbind(void);

      //! @brief Clear Material
      void Clear(void);

      //! @brief Set the material params
      inline void SetParams(float roughness, float metallic) { m_roughnessValue = roughness; m_metallicValue = metallic; }

      //! @brief Set the material name
      inline void SetName(std::string name) { m_name = name; }

      //! @brief Load Material from File (eg. "fileName.mat")
      static NightEngine::EC::Handle<Material> LoadMaterial(const std::string& fileName);

      //! @brief Save Material from File
      static void SaveMaterial(const std::string& fileName, Material& material);

      //! @brief Preload All Materials in the Path
      static void PreLoadAllMaterials(void);
    private:
      std::string m_name = "unnamed";
      Shader      m_shader;

      NightEngine::EC::Handle<Texture> m_diffuseTexture;
      glm::vec3   m_diffuseColor = glm::vec3(1.0f);

      NightEngine::EC::Handle<Texture>  m_normalTexture;
      float       m_normalMultiplier  = 1.0f;
      bool        m_useNormal = false;

      NightEngine::EC::Handle<Texture>  m_roughnessTexture;
      float 	    m_roughnessValue    = 0.01f;

      NightEngine::EC::Handle<Texture>  m_metallicTexture;
      float 	    m_metallicValue     = 0.1f;

      NightEngine::EC::Handle<Texture> m_emissiveTexture;
      float 	    m_emissiveStrength = 15.0f;
  };
}