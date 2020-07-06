/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Header of Material
*/
#pragma once

#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/MaterialProperty.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/EC/Handle.hpp"

#include <unordered_map>
#include <map>

#define TEXTURE_TABLE(TYPE) std::unordered_map<int, TYPE>
#define PROPERTY_TABLE(TYPE) std::map<std::string, TYPE>  //use std::map for tao::json native serialization support

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
      void	InitPBRTexture(const std::string& diffuseTextureFile
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
      inline Shader& GetShader(void) { return m_shader; }

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

      //////////////////////////////////////////////////////////////

      inline TEXTURE_TABLE(NightEngine::EC::Handle<Texture>)& GetTextureMap(void) { return m_textureMap; }

      inline PROPERTY_TABLE(glm::vec4)& GetVec4Map(void) { return m_vec4Map; }

      inline PROPERTY_TABLE(float)& GetFloatMap(void) { return m_floatMap; }

      inline PROPERTY_TABLE(int)& GetIntMap(void) { return m_intMap; }

      const MaterialProperty* GetMaterialProperty(void) { return m_materialProperty; }
    private:
      std::string m_name = "unnamed";
      std::string m_filePath = "";
      Shader      m_shader;

      const MaterialProperty* m_materialProperty = nullptr;

      TEXTURE_TABLE(NightEngine::EC::Handle<Texture>) m_textureMap;
      PROPERTY_TABLE(glm::vec4) m_vec4Map;
      PROPERTY_TABLE(float)     m_floatMap;
      PROPERTY_TABLE(int)       m_intMap;
  };
}