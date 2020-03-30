/*!
  @file Material.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of File
*/

#include "Graphics/Opengl/Material.hpp"

#include "Core/Serialization/Serialization.hpp"
#include "Core/Serialization/SerializeFunction.hpp"
#include "Core/Serialization/ResourceManager.hpp"

using namespace NightEngine;

namespace Rendering
{
  INIT_REFLECTION_FOR(Material)

  Material::Material(const Material& rhs)
  {
    *this = rhs;
  }

  Material& Material::operator=(const Material& rhs)
  {
    m_shader = rhs.m_shader;

    m_diffuseTexture = rhs.m_diffuseTexture;
    m_normalTexture = rhs.m_normalTexture;
    m_normalMultiplier = rhs.m_normalMultiplier;
    m_useNormal = rhs.m_useNormal;

    m_roughnessTexture = rhs.m_roughnessTexture;
    m_roughnessValue = rhs.m_roughnessValue;

    m_metallicTexture = rhs.m_metallicTexture;
    m_metallicValue = rhs.m_metallicValue;

    m_emissiveTexture = rhs.m_emissiveTexture;
    m_emissiveStrength = rhs.m_emissiveStrength;

    return *this;
  }

  void	Material::InitShader(const std::string& vertexShader
  , const std::string& fragmentShader)
  {
    m_shader.Create();
    m_shader.AttachShaderFile(vertexShader);
    m_shader.AttachShaderFile(fragmentShader);
    m_shader.Link();
  }

  void	Material::InitShader(const std::string& vertexShader
    , const std::string& fragmentShader, const std::string& geometryShader)
  {
    m_shader.Create();
    m_shader.AttachShaderFile(vertexShader);
    m_shader.AttachShaderFile(fragmentShader);
    m_shader.AttachShaderFile(geometryShader);
    m_shader.Link();
  }

  void Material::InitTexture(const std::string & diffuseTextureFile
    , bool useNormal ,const std::string& normalTextureFile
    , const std::string& roughnessTextureFile
    , const std::string& metallicTexture
    , const std::string& emissiveTextureFile)
  {
    m_useNormal = useNormal;
    RefreshTextureUniforms();

    if (diffuseTextureFile.size() > 0)
    {
      m_diffuseTexture = Texture(diffuseTextureFile
      , Texture::Channel::SRGB);
    }
    if (normalTextureFile.size() > 0)
    {
      m_normalTexture = Texture(normalTextureFile
        , Texture::Channel::RGB);
    }
    if (roughnessTextureFile.size() > 0)
    {
      m_roughnessTexture = Texture(roughnessTextureFile
        , Texture::Channel::RGB);
    }
    if (metallicTexture.size() > 0)
    {
      m_metallicTexture = Texture(metallicTexture
        , Texture::Channel::RGB);
    }
    if (emissiveTextureFile.size() > 0)
    {
      m_emissiveTexture = Texture(emissiveTextureFile
        , Texture::Channel::RGB);
    }
  }

  void Material::RefreshTextureUniforms(void) const
  {
    m_shader.Bind();
    {
      m_shader.SetUniform("u_material.m_normalMultiplier", m_normalMultiplier);
      m_shader.SetUniform("u_material.m_diffuseMap", 0);
      m_shader.SetUniform("u_material.m_normalMap", 1);
      m_shader.SetUniform("u_material.m_roughnessMap", 2);
      m_shader.SetUniform("u_material.m_metallicMap", 3);
      m_shader.SetUniform("u_material.m_emissiveMap", 4);
    }
    m_shader.Unbind();
  }

  void Material::Bind(bool useTexture)
  {
    m_shader.Bind();

    if (useTexture)
    {
      m_diffuseTexture.BindToTextureUnit(0);
      m_shader.SetUniform("u_diffuseColor", m_diffuseColor);
      
      m_shader.SetUniform("u_useNormalmap", m_useNormal);
      if (m_useNormal)
      {
        m_shader.SetUniform("u_material.m_normalMultiplier", m_normalMultiplier);
        m_normalTexture.BindToTextureUnit(1);
      }

      m_roughnessTexture.BindToTextureUnit(2);
      m_shader.SetUniform("u_material.m_roughnessValue", m_roughnessValue);

      m_metallicTexture.BindToTextureUnit(3);
      m_shader.SetUniform("u_material.m_metallicValue", m_metallicValue);

      m_emissiveTexture.BindToTextureUnit(4);
      m_shader.SetUniform("u_material.m_emissiveStrength", m_emissiveStrength);
    }
  }

  void Material::Unbind(void)
  {
    m_shader.Unbind();
  }

  void Material::Clear(void)
  {
    m_shader.Clear();

    m_diffuseTexture.Clear();
    m_normalTexture.Clear();
    m_roughnessTexture.Clear();
    m_metallicTexture.Clear();
    m_emissiveTexture.Clear();
  }

  EC::Handle<Material> Material::LoadMaterial(const std::string& fileName)
  {
    Debug::Log << Logger::MessageType::INFO
      << "***********************************\n";
    Debug::Log << Logger::MessageType::INFO
      << "Loading Material: " << fileName << '\n';

    //Load the Material through ResourceManager
    auto newMat = ResourceManager::LoadMaterialResource(fileName);
    ASSERT_TRUE(newMat.IsValid());

    Debug::Log << Logger::MessageType::INFO
      << "Loaded Material: " << fileName << '\n';
    Debug::Log << Logger::MessageType::INFO
      << "***********************************\n";

    return newMat;
  }

  void Material::SaveMaterial(const std::string& fileName, Material& material)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Saving Material: " << fileName << '\n';

    Serialization::SerializeToFile(material
      , fileName
      , FileSystem::DirectoryType::Materials);

    Debug::Log << Logger::MessageType::INFO 
      << "Saved Material: " << fileName << '\n';
  }
}