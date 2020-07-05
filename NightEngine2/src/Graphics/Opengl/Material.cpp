/*!
  @file Material.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Material
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
    m_name = rhs.m_name;
    m_filePath = rhs.m_filePath;
    m_shader = rhs.m_shader;

    m_textureMap = rhs.m_textureMap;
    m_colorMap = rhs.m_colorMap;
    m_vec4Map = rhs.m_vec4Map;
    m_floatMap = rhs.m_floatMap;
    m_intMap = rhs.m_intMap;

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

  void Material::InitTexture(const std::string& diffuseTextureFile
    , bool useNormal, const std::string& normalTextureFile
    , const std::string& roughnessTextureFile
    , const std::string& metallicTextureFile
    , const std::string& emissiveTextureFile)
  {
    //Init PBR Vals
    m_intMap[MP_PBRMetallic::u_useNormalmap] = useNormal;

    auto it = m_floatMap.find(MP_PBRMetallic::m_normalMultiplier);
    if (it == m_floatMap.end())
    {
      m_floatMap[MP_PBRMetallic::m_normalMultiplier] = 1.0f;
    }

    it = m_floatMap.find(MP_PBRMetallic::m_roughnessValue);
    if (it == m_floatMap.end())
    {
      m_floatMap[MP_PBRMetallic::m_roughnessValue] = 0.5f;
    }

    it = m_floatMap.find(MP_PBRMetallic::m_metallicValue);
    if (it == m_floatMap.end())
    {
      m_floatMap[MP_PBRMetallic::m_metallicValue] = 0.1f;
    }

    it = m_floatMap.find(MP_PBRMetallic::m_emissiveStrength);
    if (it == m_floatMap.end())
    {
      m_floatMap[MP_PBRMetallic::m_emissiveStrength] = 15.0f;
    }

    auto it2 = m_colorMap.find(MP_PBRMetallic::u_diffuseColor);
    if (it2 == m_colorMap.end())
    {
      m_colorMap[MP_PBRMetallic::u_diffuseColor] = glm::vec4(1.0f);
    }

    //Init Textures
    if (diffuseTextureFile.size() > 0)
    {
      auto diffuseTexture = Texture::LoadTextureHandle(diffuseTextureFile
      , Texture::Channel::SRGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = diffuseTexture;
    }
    else
    {
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = ResourceManager::GetWhiteTexture();
    }

    if (normalTextureFile.size() > 0)
    {
      auto normalTexture = Texture::LoadTextureHandle(normalTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRINEAREST);
      m_textureMap[NORMAL_TEXUNIT_INDEX] = normalTexture;
    }
    else
    {
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = ResourceManager::GetBlackTexture();
    }

    if (roughnessTextureFile.size() > 0)
    {
      auto roughnessTexture = Texture::LoadTextureHandle(roughnessTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[ROUGHNESS_TEXUNIT_INDEX] = roughnessTexture;
    }
    else
    {
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = ResourceManager::GetWhiteTexture();
    }

    if (metallicTextureFile.size() > 0)
    {
      auto metallicTexture = Texture::LoadTextureHandle(metallicTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[METALLIC_TEXUNIT_INDEX] = metallicTexture;
    }
    else
    {
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = ResourceManager::GetBlackTexture();
    }

    if (emissiveTextureFile.size() > 0)
    {
      auto emissiveTexture = Texture::LoadTextureHandle(emissiveTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[EMISSIVE_TEXUNIT_INDEX] = emissiveTexture;
    }
    else
    {
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = ResourceManager::GetBlackTexture();
    }

    RefreshTextureUniforms();
  }

  void Material::RefreshTextureUniforms(void) const
  {
    m_shader.Bind();
    {
      auto it = m_floatMap.find(MP_PBRMetallic::m_normalMultiplier);
      if (it != m_floatMap.end())
      {
        auto val = it->second;
        m_shader.SetUniform(MP_PBRMetallic::m_normalMultiplier, val);
      }

      MP_PBRMetallic::SetTextureBindingUnit(m_shader);
    }
    m_shader.Unbind();
  }

  void Material::Bind(bool useTexture)
  {
    m_shader.Bind();

    if (useTexture)
    {
      for (auto& pair : m_textureMap)
      {
        if (pair.second.IsValid() && pair.second->IsValid())
        {
          pair.second->BindToTextureUnit(pair.first);
        }
        else
        {
          ResourceManager::GetWhiteTexture()->BindToTextureUnit(pair.first);
        }
      }

      for (auto& pair : m_colorMap)
      {
        m_shader.SetUniform(pair.first, pair.second);
      }

      for (auto& pair : m_vec4Map)
      {
        m_shader.SetUniform(pair.first, pair.second);
      }

      for (auto& pair : m_floatMap)
      {
        m_shader.SetUniform(pair.first, pair.second);
      }

      for (auto& pair : m_intMap)
      {
        m_shader.SetUniform(pair.first, pair.second);
      }
    }
  }

  void Material::Unbind(void)
  {
    m_shader.Unbind();
  }

  void Material::Clear(void)
  {
    m_shader.Clear();

    for (auto& tex : m_textureMap)
    {
      tex.second->Clear();
    }
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

  void Material::PreLoadAllMaterials(void)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Material::PreLoadAllMaterials()\n";

    static std::vector <std::string> s_items;

    FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Materials, s_items
      , FileSystem::FileFilter::FileName, ".mat", false, true, true);

    for (int i = 0; i < s_items.size(); ++i)
    {
      auto newHandle = ResourceManager::LoadMaterialResource(s_items[i]);
      bool success = newHandle.IsValid();
      {
        Debug::Log << (success? Logger::MessageType::INFO: Logger::MessageType::WARNING)
          << "Loading Material: " << s_items[i]
          << (success ? " successfully\n": " unsuccessfully\n");
      }
    }
  }

  void Material::SetParams(float roughness, float metallic) 
  {
    m_floatMap[MP_PBRMetallic::m_roughnessValue] = roughness;
    m_floatMap[MP_PBRMetallic::m_metallicValue] = metallic;
  }
}