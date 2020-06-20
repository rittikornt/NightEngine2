/*!
	@file SerializeFunction.cpp
	@author Rittikorn Tangtrongchit
	@brief Contain the Implementation of SerializeFunction
*/

#include "SerializeFunction.hpp"

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

      //Diffuse Texture
      if (material.m_diffuseTexture.IsValid()
        && material.m_diffuseTexture->m_filePath.size() > 0)
      {
        JsonValue diffuseValue = material.m_diffuseTexture->m_filePath;
        value.emplace("Diffuse", diffuseValue);
      }

      //Normal Texture
      if (material.m_normalTexture.IsValid()
        && material.m_normalTexture->m_filePath.size() > 0)
      {
        JsonValue normalValue = material.m_normalTexture->m_filePath;
        value.emplace("Normal", normalValue);
      }
      JsonValue normalMultiplierValue = material.m_normalMultiplier;
      value.emplace("Normal Multiplier", normalMultiplierValue);

      JsonValue useNormalValue = material.m_useNormal;
      value.emplace("UseNormal", useNormalValue);

      //Roughness Texture
      if (material.m_roughnessTexture.IsValid()
        && material.m_roughnessTexture->m_filePath.size() > 0)
      {
        JsonValue roughnessFilePath = material.m_roughnessTexture->m_filePath;
        value.emplace("Roughness", roughnessFilePath);
      }
      JsonValue roughnessValue = material.m_roughnessValue;
      value.emplace("Roughness Value", roughnessValue);

      //Metallic Texture
      if (material.m_metallicTexture.IsValid()
        && material.m_metallicTexture->m_filePath.size() > 0)
      {
        JsonValue metallicFilePath = material.m_metallicTexture->m_filePath;
        value.emplace("Metallic", metallicFilePath);
      }
      JsonValue metallicValue = material.m_metallicValue;
      value.emplace("Metallic Value", metallicValue);

      //Emissive Texture
      if (material.m_emissiveTexture.IsValid() 
        && material.m_emissiveTexture->m_filePath.size() > 0)
      {
        JsonValue emissiveValue = material.m_emissiveTexture->m_filePath;
        value.emplace("Emissive", emissiveValue);
      }
      JsonValue emissiveStrValue = material.m_emissiveStrength;
      value.emplace("Emissive Strength", emissiveStrValue);

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

      //Normal Values
      it = obj.find("Normal Multiplier");
      if (it != obj.end())
      {
        material.m_normalMultiplier = it->second.as<float>();
      }
      it = obj.find("UseNormal");
      if (it != obj.end())
      {
        material.m_useNormal = it->second.as<bool>();
      }

      //Roughness/Metallic Values
      it = obj.find("Roughness Value");
      if (it != obj.end())
      {
        material.m_roughnessValue = it->second.as<float>();
      }
      it = obj.find("Metallic Value");
      if (it != obj.end())
      {
        material.m_metallicValue = it->second.as<float>();
      }

      //Emissive Values
      it = obj.find("Emissive Strength");
      if (it != obj.end())
      {
        material.m_emissiveStrength = it->second.as<float>();
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
      material.InitTexture(diffuseFile
        , material.m_useNormal, normalFile
        , roughnessFile, metallicFile, emissiveFile);
    }
	}
}
