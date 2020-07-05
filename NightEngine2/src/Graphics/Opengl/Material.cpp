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

#define DIFFUSE_TEXUNIT_INDEX 0
#define NORMAL_TEXUNIT_INDEX 1
#define ROUGHNESS_TEXUNIT_INDEX 2
#define METALLIC_TEXUNIT_INDEX 3
#define EMISSIVE_TEXUNIT_INDEX 4

namespace Rendering
{
  const char* PBRShaderParams::k_textureNames[5] = 
  { "DiffuseMap" , "NormalMap", "RoughnessMap", "MetallicMap", "EmissiveMap" };

  const char* PBRShaderParams::m_diffuseMap = "u_material.m_diffuseMap";
  const char* PBRShaderParams::u_diffuseColor = "u_diffuseColor";

  const char* PBRShaderParams::u_useNormalmap = "u_useNormalmap";
  const char* PBRShaderParams::m_normalMultiplier = "u_material.m_normalMultiplier";
  const char* PBRShaderParams::m_normalMap = "u_material.m_normalMap";

  const char* PBRShaderParams::m_roughnessMap = "u_material.m_roughnessMap";
  const char* PBRShaderParams::m_roughnessValue = "u_material.m_roughnessValue";

  const char* PBRShaderParams::m_metallicMap = "u_material.m_metallicMap";
  const char* PBRShaderParams::m_metallicValue = "u_material.m_metallicValue";

  const char* PBRShaderParams::m_emissiveMap = "u_material.m_emissiveMap";
  const char* PBRShaderParams::m_emissiveStrength = "u_material.m_emissiveStrength";
}

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
    m_vec4Map = rhs.m_vec4Map;
    m_floatMap = rhs.m_floatMap;
    m_intMap = rhs.m_intMap;

    //m_diffuseTexture = rhs.m_diffuseTexture;
    //m_diffuseColor = rhs.m_diffuseColor;
    //
    //m_normalTexture = rhs.m_normalTexture;
    //m_normalMultiplier = rhs.m_normalMultiplier;
    //m_useNormal = rhs.m_useNormal;
    //
    //m_roughnessTexture = rhs.m_roughnessTexture;
    //m_roughnessValue = rhs.m_roughnessValue;
    //
    //m_metallicTexture = rhs.m_metallicTexture;
    //m_metallicValue = rhs.m_metallicValue;
    //
    //m_emissiveTexture = rhs.m_emissiveTexture;
    //m_emissiveStrength = rhs.m_emissiveStrength;

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
    m_intMap[PBRShaderParams::u_useNormalmap] = useNormal;

    auto it = m_floatMap.find(PBRShaderParams::m_normalMultiplier);
    if (it == m_floatMap.end())
    {
      m_floatMap[PBRShaderParams::m_normalMultiplier] = 1.0f;
    }

    it = m_floatMap.find(PBRShaderParams::m_roughnessValue);
    if (it == m_floatMap.end())
    {
      m_floatMap[PBRShaderParams::m_roughnessValue] = 0.5f;
    }

    it = m_floatMap.find(PBRShaderParams::m_metallicValue);
    if (it == m_floatMap.end())
    {
      m_floatMap[PBRShaderParams::m_metallicValue] = 0.1f;
    }

    it = m_floatMap.find(PBRShaderParams::m_emissiveStrength);
    if (it == m_floatMap.end())
    {
      m_floatMap[PBRShaderParams::m_emissiveStrength] = 15.0f;
    }

    //m_useNormal = useNormal;
    RefreshTextureUniforms();

    if (diffuseTextureFile.size() > 0)
    {
      auto diffuseTexture = Texture::LoadTextureHandle(diffuseTextureFile
      , Texture::Channel::SRGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[DIFFUSE_TEXUNIT_INDEX] = diffuseTexture;
    }
    if (normalTextureFile.size() > 0)
    {
      auto normalTexture = Texture::LoadTextureHandle(normalTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRINEAREST);
      m_textureMap[NORMAL_TEXUNIT_INDEX] = normalTexture;
    }
    if (roughnessTextureFile.size() > 0)
    {
      auto roughnessTexture = Texture::LoadTextureHandle(roughnessTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[ROUGHNESS_TEXUNIT_INDEX] = roughnessTexture;
    }
    if (metallicTextureFile.size() > 0)
    {
      auto metallicTexture = Texture::LoadTextureHandle(metallicTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[METALLIC_TEXUNIT_INDEX] = metallicTexture;
    }
    if (emissiveTextureFile.size() > 0)
    {
      auto emissiveTexture = Texture::LoadTextureHandle(emissiveTextureFile
        , Texture::Channel::RGB, Texture::FilterMode::TRILINEAR);
      m_textureMap[EMISSIVE_TEXUNIT_INDEX] = emissiveTexture;
    }
  }

  void Material::RefreshTextureUniforms(void) const
  {
    m_shader.Bind();
    {
      auto it = m_floatMap.find(PBRShaderParams::m_normalMultiplier);
      if (it != m_floatMap.end())
      {
        auto val = it->second;
        m_shader.SetUniform(PBRShaderParams::m_normalMultiplier, val);
      }

      m_shader.SetUniform(PBRShaderParams::m_diffuseMap, 0);
      m_shader.SetUniform(PBRShaderParams::m_normalMap, 1);
      m_shader.SetUniform(PBRShaderParams::m_roughnessMap, 2);
      m_shader.SetUniform(PBRShaderParams::m_metallicMap, 3);
      m_shader.SetUniform(PBRShaderParams::m_emissiveMap, 4);
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

      /*{
        if (m_diffuseTexture.IsValid())
        {
          m_diffuseTexture->BindToTextureUnit(0);
        }
        else
        {
          ResourceManager::GetWhiteTexture()->BindToTextureUnit(0);
        }
        m_shader.SetUniform(PBRShaderParams::u_diffuseColor, m_diffuseColor);

        m_shader.SetUniform(PBRShaderParams::u_useNormalmap, m_useNormal);
        if (m_useNormal)
        {
          m_shader.SetUniform(PBRShaderParams::m_normalMap, m_normalMultiplier);
          if (m_normalTexture.IsValid())
          {
            m_normalTexture->BindToTextureUnit(1);
          }
        }

        if (m_roughnessTexture.IsValid())
        {
          m_roughnessTexture->BindToTextureUnit(2);
        }
        else
        {
          ResourceManager::GetWhiteTexture()->BindToTextureUnit(2);
        }
        m_shader.SetUniform(PBRShaderParams::m_roughnessValue, m_roughnessValue);

        if (m_metallicTexture.IsValid())
        {
          m_metallicTexture->BindToTextureUnit(3);
        }
        else
        {
          ResourceManager::GetBlackTexture()->BindToTextureUnit(3);
        }
        m_shader.SetUniform(PBRShaderParams::m_metallicValue, m_metallicValue);

        if (m_emissiveTexture.IsValid())
        {
          m_emissiveTexture->BindToTextureUnit(4);
        }
        else
        {
          ResourceManager::GetWhiteTexture()->BindToTextureUnit(4);
        }
        m_shader.SetUniform(PBRShaderParams::m_emissiveStrength, m_emissiveStrength);
      }*/
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

    //m_diffuseTexture->Clear();
    //m_normalTexture->Clear();
    //m_roughnessTexture->Clear();
    //m_metallicTexture->Clear();
    //m_emissiveTexture->Clear();
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
    m_floatMap[PBRShaderParams::m_roughnessValue] = roughness;
    m_floatMap[PBRShaderParams::m_metallicValue] = metallic;
  }
}