/*!
  @file Serialization.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Serialization
*/
#pragma once
#include "Core/Serialization/FileSystem.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Reflection/MetaType.hpp"
#include "Core/Reflection/Variable.hpp"

#include "taocpp_json/include/tao/json/stream.hpp"
#include "taocpp_json/include/tao/json/from_string.hpp"
#include "taocpp_json/include/tao/json/as.hpp"
#include "taocpp_json/include/tao/json/basic_value.hpp"

namespace NightEngine
{
  namespace Serialization
  {
		///////////////////////////////////////////////////////////////////////////

		//! @brief Serialization Function
		template<typename T>
		void Serialize(const T& objectToSerialize, std::ostream& out)
		{
			using namespace NightEngine::Reflection;
			//Variable for type T
			MetaType* metaType = METATYPE(T);
			Variable var{ metaType, const_cast<T*>(&objectToSerialize) };

			//Json to ostream
			JsonValue value{ { metaType->GetName(),var.Serialize() } };
			tao::json::to_stream(out, value, 2);
		}

		//! @brief Serialization Function for specifying serialize function
		template<typename T>
		void Serialize(const T& objectToSerialize, std::ostream& out
		, typename NightEngine::Reflection::MetaType::SerializeFn function)
		{
			using namespace NightEngine::Reflection;
			//Variable for type T
			MetaType* metaType = METATYPE(T);
			Variable var{ metaType, const_cast<T*>(&objectToSerialize) };

			//Json to ostream
			tao::json::to_stream(out, function(var), 2);
		}

		//! @brief Serialize Object to file
		template<typename T>
		void SerializeToFile(const T& objectToSerialize
			, const std::string& fileName, FileSystem::DirectoryType dirType)
		{
			auto file = FileSystem::CreateFileTo(fileName, dirType, false);
			Serialize(objectToSerialize, *file);
			file->close();
		}

		///////////////////////////////////////////////////////////////////////////

		//! @brief Deserialize file into outObject, T must have default constructor
		template<typename T>
		void Deserialize(T& outObject, const std::string& fileName
			, FileSystem::DirectoryType dirType)
		{
			using namespace NightEngine::Reflection;
			using namespace tao::json;

			//Variable for type T
			MetaType* metaType = METATYPE(T);
			//std::unique_ptr<T> tempObject{ new T };
			Variable var{ metaType, &outObject };
			
			std::string jsonFile = FileSystem::OpenFileAsString(fileName, dirType);
			JsonValue value = tao::json::from_string(jsonFile);
			auto map = value.get_object();
			
			//Deserialize Object for type T
			auto it = map.begin();
			if (it->first == metaType->GetName())
			{
				var.Deserialize(it->second);
			}
		}

		//TODO: Test Serialize/Deserialize with custom Function
		//! @brief Deserialize file into outObject with custom Deserialize function
		template<typename T>
		void Deserialize(T& outObject, const std::string& fileName
			, FileSystem::DirectoryType dirType
			, typename NightEngine::Reflection::MetaType::DeserializeFn function)
		{
			using namespace NightEngine::Reflection;
			using namespace tao::json;

			//Variable for type T
			MetaType* metaType = METATYPE(T);
			//std::unique_ptr<T> tempObject{ new T };
			Variable var{ metaType, &outObject };

			std::string jsonFile = FileSystem::OpenFileAsString(fileName, dirType);
			JsonValue value = tao::json::from_string(jsonFile);
			auto map = value.get_object();

			//Deserialize Object for type T
			auto it = map.begin();
			if (it->first == metaType->GetName())
			{
				function(it->second, var);
			}
		}
	}
}