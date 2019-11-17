/*!
  @file Material.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of File
*/
#pragma once

#include "Graphic/Texture.hpp"
#include "Graphic/Shader.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

namespace Graphic
{
  class Material
  {
    REFLECTABLE_TYPE();

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

      //! @brief Get Shader
      Shader& GetShader(void) { return m_shader; }

      //! @brief Apply material to the shader
      void Bind(bool useTexture = true);

      //! @brief Unbind the Shader
      void Unbind(void);

      //! @brief Load Material from File
      static Material* LoadMaterial(const std::string& fileName);

      //! @brief Save Material from File
      static void SaveMaterial(const std::string& fileName, Material& material);
    private:
      Shader    m_shader;

      Texture   m_diffuseTexture;
      glm::vec3 m_diffuseColor = glm::vec3(1.0f);

      Texture   m_normalTexture;
      float     m_normalMultiplier  = 1.0f;
      bool      m_useNormal = false;

      Texture   m_roughnessTexture;
      float 	  m_roughnessValue    = 0.0f;

      Texture   m_metallicTexture;
      float 	  m_metallicValue     = 0.0f;

      Texture   m_emissiveTexture;
      float 	  m_emissiveStrength = 15.0f;
  };
  
}