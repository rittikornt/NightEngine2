/*!
	@file SerializeFunction.cpp
	@author Rittikorn Tangtrongchit
	@brief Contain the Implementation of SerializeFunction
*/

#include "SerializeFunction.hpp"
#include "Core/Serialization/FileSystem.hpp"

#include "Core/Macros.hpp"
#include "Core/Reflection/MetaType.hpp"

#include "Core/EC/GameObject.hpp"

#include "Graphics/Opengl/Material.hpp"

//JsonValue
#include "taocpp_json/include/tao/json/value.hpp"

//***********************************************
//	Macros
//***********************************************
#define DEFINE_DEFAULT_SERIALIZER(TYPE)\
template<> JsonValue DefaultSerializer<TYPE>(Reflection::Variable& variable)\
{\
	ASSERT_TRUE(variable.GetMetaType() != nullptr);\
	JsonValue val = variable.GetValue<TYPE>();\
	return val;\
}

#define DEFINE_DEFAULT_SERIALIZER_WITH_TYPE(TYPE)\
template<> JsonValue DefaultSerializer<TYPE>(Reflection::Variable& variable)\
{\
		return JsonValue{ { variable.GetMetaType()->GetName()\
		, variable.GetValue<TYPE>() } };\
}

#define DEFINE_DEFAULT_DESERIALIZER(TYPE)\
template<> void DefaultDeserializer<TYPE>(ValueObject& valueObject, Reflection::Variable& variable)\
{\
	ASSERT_TRUE(variable.GetMetaType() != nullptr);\
	TYPE val = valueObject.as<TYPE>();\
	variable.SetValue(val);\
}

using JsonValue = tao::json::value;

using namespace NightEngine::EC;
using namespace NightEngine::Reflection;

using namespace Rendering;

namespace NightEngine
{
	namespace Serialization
	{
    DEFINE_DEFAULT_SERIALIZER(bool)
    DEFINE_DEFAULT_SERIALIZER(int)
    DEFINE_DEFAULT_SERIALIZER(unsigned)
    DEFINE_DEFAULT_SERIALIZER(float)
    DEFINE_DEFAULT_SERIALIZER(double)
    DEFINE_DEFAULT_SERIALIZER(unsigned long long)
    DEFINE_DEFAULT_SERIALIZER(std::string)

    template<> 
    JsonValue DefaultSerializer<GameObject&>(Reflection::Variable& variable)
    {
      ASSERT_TRUE(variable.GetMetaType() != nullptr);
      JsonValue value;
      auto& gameObject = variable.GetValue<GameObject>();
      
      //Name
      Variable nameVar{ METATYPE_FROM_OBJECT(gameObject.m_name), &(gameObject.m_name) };
      value.emplace("m_name", nameVar.Serialize());

      //Components
      if (gameObject.m_components.size() > 0)
      {
        JsonValue componentValue;
        for (auto& component : gameObject.m_components)
        {
          Variable componentsVar{ component.m_metaType
            , component.GetPointer() };
          componentValue.emplace(component.m_metaType->GetName()
            , componentsVar.Serialize());
        }
        value.emplace("m_components", componentValue);
      }

      //Transform
      Variable transformVar{ METATYPE_FROM_OBJECT(*(gameObject.m_transform.Get()))
        , gameObject.m_transform.Get() };
      value.emplace("m_transform", transformVar.Serialize());

      return value;
    }

    template<>
    JsonValue DefaultSerializer<Material&>(Reflection::Variable& variable)
    {
      ASSERT_TRUE(variable.GetMetaType() != nullptr);
      JsonValue value;
      auto& material = variable.GetValue<Material>();

      //Shader Files
      auto& shaderNames = material.m_shader.m_filePath;
      if (shaderNames.size() > 0)
      {
        JsonValue shaderNameValue;
        for (auto& name : shaderNames)
        {
          auto index = name.rfind("/Shaders");
          auto fileName = name.substr(index + 9);
          shaderNameValue.append({ fileName });
        }

        //Serialize all shader file names
        value.emplace("Shaders", shaderNameValue);
      }

      //TODO: Fix the broken serialization
      //Convert texture map to <int, filePath>
      JsonValue textureMapValue;
      bool shouldEmplace = false;
      for (auto& pair : material.m_textureMap)
      {
        if (pair.second.IsValid())
        {
          TextureData texData = pair.second->GetTextureData();
          Variable texDataVar{ METATYPE_FROM_OBJECT(texData), &texData };

          textureMapValue.emplace(std::to_string(pair.first)
            , texDataVar.Serialize());
          shouldEmplace = true;
        }
        else
        {
          DEBUG_WARNING << "Error: trying to serialize invalid material property ("
            << pair.first << ")\n";
        }
      }
      if (shouldEmplace)
      {
        value.emplace("m_textureMap", textureMapValue);
      }

      //Color map
      JsonValue colorValue;
      for (auto& pair : material.m_colorMap)
      {
        Variable colVar{ METATYPE_FROM_OBJECT(pair.second), &pair.second };
        colorValue.emplace(pair.first, colVar.Serialize());
      }
      if (material.m_colorMap.size() > 0)
      {
        value.emplace("m_colorMap", colorValue);
      }

      //Vec4 map
      JsonValue vec4Value;
      for (auto& pair : material.m_vec4Map)
      {
        Variable vec4Var{ METATYPE_FROM_OBJECT(pair.second), &pair.second };
        vec4Value.emplace(pair.first, vec4Var.Serialize());
      }
      if (material.m_vec4Map.size() > 0)
      {
        value.emplace("m_vec4Map", vec4Value);
      }

      //Float/Int map
      if (material.m_floatMap.size() > 0)
      {
        value.emplace("m_floatMap", material.m_floatMap);
      }
      if (material.m_intMap.size() > 0)
      {
        value.emplace("m_intMap", material.m_intMap);
      }

      //Diffuse Color
      //Variable diffuseColorVar{ METATYPE_FROM_OBJECT(material.m_diffuseColor)
      //  , &material.m_diffuseColor };
      //value.emplace("Diffuse Color", diffuseColorVar.Serialize());
      //
      ////Diffuse Texture
      //if (material.m_diffuseTexture.IsValid()
      //  && material.m_diffuseTexture->m_filePath.size() > 0)
      //{
      //  JsonValue diffuseValue = material.m_diffuseTexture->m_filePath;
      //  value.emplace("Diffuse", diffuseValue);
      //}
      //
      ////Normal Texture
      //if (material.m_normalTexture.IsValid()
      //  && material.m_normalTexture->m_filePath.size() > 0)
      //{
      //  JsonValue normalValue = material.m_normalTexture->m_filePath;
      //  value.emplace("Normal", normalValue);
      //}
      //JsonValue normalMultiplierValue = material.m_normalMultiplier;
      //value.emplace("Normal Multiplier", normalMultiplierValue);
      //
      //JsonValue useNormalValue = material.m_useNormal;
      //value.emplace("UseNormal", useNormalValue);
      //
      ////Roughness Texture
      //if (material.m_roughnessTexture.IsValid()
      //  && material.m_roughnessTexture->m_filePath.size() > 0)
      //{
      //  JsonValue roughnessFilePath = material.m_roughnessTexture->m_filePath;
      //  value.emplace("Roughness", roughnessFilePath);
      //}
      //JsonValue roughnessValue = material.m_roughnessValue;
      //value.emplace("Roughness Value", roughnessValue);
      //
      ////Metallic Texture
      //if (material.m_metallicTexture.IsValid()
      //  && material.m_metallicTexture->m_filePath.size() > 0)
      //{
      //  JsonValue metallicFilePath = material.m_metallicTexture->m_filePath;
      //  value.emplace("Metallic", metallicFilePath);
      //}
      //JsonValue metallicValue = material.m_metallicValue;
      //value.emplace("Metallic Value", metallicValue);
      //
      ////Emissive Texture
      //if (material.m_emissiveTexture.IsValid() 
      //  && material.m_emissiveTexture->m_filePath.size() > 0)
      //{
      //  JsonValue emissiveValue = material.m_emissiveTexture->m_filePath;
      //  value.emplace("Emissive", emissiveValue);
      //}
      //JsonValue emissiveStrValue = material.m_emissiveStrength;
      //value.emplace("Emissive Strength", emissiveStrValue);

      return value;
    }

    template<>
    JsonValue DefaultSerializer<NightEngine::EC::Handle<Rendering::Material>>(Reflection::Variable& variable)
    {
      ASSERT_TRUE(variable.GetMetaType() != nullptr);
      JsonValue value;
      auto& matHandle = variable.GetValue<NightEngine::EC::Handle<Rendering::Material>>();

      //File Path
      JsonValue filePathVal = matHandle->GetFilePath();
      value.emplace("filePath", filePathVal);

      return value;
    }

		////////////////////////////////////////////////////////////

    DEFINE_DEFAULT_DESERIALIZER(bool)
    DEFINE_DEFAULT_DESERIALIZER(int)
    DEFINE_DEFAULT_DESERIALIZER(unsigned)
    DEFINE_DEFAULT_DESERIALIZER(float)
    DEFINE_DEFAULT_DESERIALIZER(double)
    DEFINE_DEFAULT_DESERIALIZER(unsigned long long)
    DEFINE_DEFAULT_DESERIALIZER(std::string)

    template <>
    void DefaultDeserializer<NightEngine::EC::GameObject&>(ValueObject& valueObject
      , Reflection::Variable& variable)
    {
      auto& gameObject = variable.GetValue<GameObject>();
      auto& obj = valueObject.get_object();

      //Find ValueObject Corresponding to member name
      auto it = obj.find("m_name");
      if (it != obj.end())
      {
        gameObject.m_name = it->second.as<Container::String>();
      }
      else
      {
        Debug::Log << Logger::MessageType::ERROR_MSG
          << "Not Found Deserialize MemberName: m_name\n";
        ASSERT_TRUE(false);
      }

      //Components
      it = obj.find("m_components");
      if (it != obj.end())
      {
        auto componentMap = it->second.get_object();
        for (auto& pair : componentMap)
        {
          //AddComponent if it doesn't exist
          auto handle = gameObject.GetComponent(pair.first.c_str());
          if (handle == nullptr)
          {
            handle = gameObject.AddComponent(pair.first.c_str());
          }

          //Deserialize into Component
          Variable componentVar{ handle->m_metaType
            ,handle->GetPointer() };
          componentVar.Deserialize(pair.second);
        }
      }
      else
      {
        Debug::Log << Logger::MessageType::WARNING
          << "Not Found Deserialize MemberName: m_componentTypes\n";
      }

      //Transform
      it = obj.find("m_transform");
      if (it != obj.end())
      {
        Variable transformVar{ METATYPE_FROM_OBJECT(*(gameObject.m_transform.Get()))
          , gameObject.m_transform.Get() };
        transformVar.Deserialize(it->second);
      }
      else
      {
        Debug::Log << Logger::MessageType::ERROR_MSG
          << "Not Found Deserialize MemberName: m_transform\n";
        ASSERT_TRUE(false);
      }
    }

    template <>
    void DefaultDeserializer<Material&>(ValueObject& valueObject
      , Reflection::Variable& variable)
    {
      auto& material = variable.GetValue<Material>();
      auto& obj = valueObject.get_object();

      //Find ValueObject Corresponding to Shaders
      auto it = obj.find("Shaders");
      if (it != obj.end())
      {
        auto array = it->second.get_array();
        std::string vertShader;
        std::string fragShader;
        std::string geomShader;

        //Init shader with file name
        for (auto& filename : array)
        {
          auto name = filename.as<std::string>();
          auto index = name.rfind(".");
          auto ext = name.substr(index + 1);
          if (ext == "vert")
          {
            vertShader = name;
          }
          else if (ext == "frag")
          {
            fragShader = name;
          }
          else if (ext == "geom")
          {
            geomShader = name;
          }
        }

        if (geomShader.size() > 0)
        {
          material.InitShader(vertShader, fragShader, geomShader);
        }
        else
        {
          material.InitShader(vertShader, fragShader);
        }
      }
      else
      {
        Debug::Log << Logger::MessageType::ERROR_MSG
        << "Not Found Deserialize MemberName: Shaders\n";
        ASSERT_TRUE(false);
      }

      //Deserializing Property Map
      it = obj.find("m_textureMap");
      if (it != obj.end())
      {
        auto map = it->second.get_object();
        for (auto& pair : map)
        {
          TextureData texData;
          Variable texDataVar{ METATYPE_FROM_OBJECT(texData), &texData };
          texDataVar.Deserialize(pair.second);

          //Creating Texture
          int bindingUnit = std::stoi(pair.first);
          material.m_textureMap[bindingUnit] = Texture::LoadTextureHandle(texData.m_filePath
            , (Texture::Channel)texData.m_channel, (Texture::FilterMode)texData.m_filterMode);
        }
      }

      it = obj.find("m_colorMap");
      if (it != obj.end())
      {
        auto map = it->second.get_object();
        for (auto& pair : map)
        {
          glm::vec4 color;
          Variable colorVar{ METATYPE_FROM_OBJECT(color), &color };
          colorVar.Deserialize(pair.second);

          material.m_colorMap[pair.first] = color;
        }
      }

      it = obj.find("m_vec4Map");
      if (it != obj.end())
      {
        auto map = it->second.get_object();
        for (auto& pair : map)
        {
          glm::vec4 v4;
          Variable v4Var{ METATYPE_FROM_OBJECT(v4), &v4 };
          v4Var.Deserialize(pair.second);

          material.m_vec4Map[pair.first] = v4;
        }
      }

      it = obj.find("m_floatMap");
      if (it != obj.end())
      {
        auto map = it->second.get_object();
        for (auto& pair : map)
        {
          material.m_floatMap[pair.first] = pair.second.as<float>();
        }
      }
      
      it = obj.find("m_intMap");
      if (it != obj.end())
      {
        auto map = it->second.get_object();
        for (auto& pair : map)
        {
          material.m_intMap[pair.first] = pair.second.as<int>();
        }
      }

      //Diffuse Color
      it = obj.find("Diffuse Color");
      if (it != obj.end())
      {
        glm::vec3 color;
        Variable diffuseColorVar{ METATYPE_FROM_OBJECT(color), & color };
        diffuseColorVar.Deserialize(it->second);

        material.m_colorMap[MP_PBRMetallic::u_diffuseColor] = glm::vec4(color, 1.0f);
      }

      //Normal Values
      it = obj.find("Normal Multiplier");
      if (it != obj.end())
      {
        auto normalMultiplier = it->second.as<float>();
        material.m_floatMap[MP_PBRMetallic::m_normalMultiplier] = normalMultiplier;
      }

      bool useNormal = false;
      it = obj.find("UseNormal");
      if (it != obj.end())
      {
        useNormal = it->second.as<bool>();
      }

      //Roughness/Metallic Values
      it = obj.find("Roughness Value");
      if (it != obj.end())
      {
        auto roughnessValue = it->second.as<float>();
        material.m_floatMap[MP_PBRMetallic::m_roughnessValue] = roughnessValue;
      }
      it = obj.find("Metallic Value");
      if (it != obj.end())
      {
        auto metallicValue = it->second.as<float>();
        material.m_floatMap[MP_PBRMetallic::m_metallicValue] = metallicValue;
      }

      //Emissive Values
      it = obj.find("Emissive Strength");
      if (it != obj.end())
      {
        auto emissiveStrength = it->second.as<float>();
        material.m_floatMap[MP_PBRMetallic::m_emissiveStrength] = emissiveStrength;
      }

      //Textures
      std::string diffuseFile;
      std::string normalFile;
      std::string roughnessFile;
      std::string metallicFile;
      std::string emissiveFile;

      it = obj.find("Diffuse");
      if (it != obj.end())
      {
        diffuseFile = it->second.as<std::string>();
      }
      it = obj.find("Normal");
      if (it != obj.end())
      {
        normalFile = it->second.as<std::string>();
      }
      it = obj.find("Roughness");
      if (it != obj.end())
      {
        roughnessFile = it->second.as<std::string>();
      }
      it = obj.find("Metallic");
      if (it != obj.end())
      {
        metallicFile = it->second.as<std::string>();
      }
      it = obj.find("Emissive");
      if (it != obj.end())
      {
        emissiveFile = it->second.as<std::string>();
      }

      //Initialize Material Texture
      material.InitPBRTexture(diffuseFile
        , useNormal, normalFile
        , roughnessFile, metallicFile, emissiveFile);
    }

    template <>
    void DefaultDeserializer<NightEngine::EC::Handle<Rendering::Material>>(ValueObject& valueObject
      , Reflection::Variable& variable)
    {
      auto& matHandle = variable.GetValue<NightEngine::EC::Handle<Rendering::Material>>();
      auto& obj = valueObject.get_object();

      //File Path
      auto it = obj.find("filePath");
      if (it != obj.end())
      {
        auto filePath = it->second.as<std::string>();
        FileSystem::RemoveFileDirectoryPath(filePath, FileSystem::DirectoryType::Materials);
        if (filePath != "")
        {
          matHandle = Material::LoadMaterial(filePath);
        }
      }
    }
	}
}
