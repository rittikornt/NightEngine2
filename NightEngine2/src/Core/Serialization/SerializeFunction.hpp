/*!
  @file SerializeFunction.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SerializeFunction
*/

#pragma once
#include "Core/Macros.hpp"

#include "Core/Reflection/MetaType.hpp"
#include "Core/Reflection/Variable.hpp"
#include "Core/Reflection/RemoveQualifier.hpp"

//JsonValue
#include "Core/Serialization/taocpp_json/include/tao/json/value.hpp"

// Expand into 1 Argument
#define EXPAND_1ARG(...) __VA_ARGS__

#define DECLARE_DEFAULT_SERIALIZER(TYPE) \
template <> \
JsonValue DefaultSerializer<TYPE>(Reflection::Variable& variable)

#define DECLARE_DEFAULT_DESERIALIZER(TYPE) \
template <> \
void DefaultDeserializer<TYPE>(ValueObject& valueObject, Reflection::Variable& variable)

namespace Graphic
{
  class Material;
}

namespace Core
{
  namespace ECS
  {
    class GameObject;
  }

  namespace Serialization
  {
		//***********************************************************************
		// Serialize
		//***********************************************************************

		//! @brief Default Serialize Function
		template <typename T>
		inline JsonValue DefaultSerializer(Reflection::Variable& variable)
		{
			using namespace Reflection;
			//Value to hold all serialize data
			JsonValue value;

			auto metaType = variable.GetMetaType();
			auto& members = metaType->GetMembers();

			// Trying to Serialize non-primitive Type 
			// with no member is an error
			ASSERT_TRUE(members.size() > 0);

			//Serialize all the members
			for (auto& member : members)
			{
        if (member.GetMetaType()->ShouldSerialized()
          && member.ShouldSerialized())
        {
          //Actual Member data by offset from Object data pointer
          void* memberPtr = POINTER_OFFSET(variable.GetValue()
            , member.GetOffset());

          //Insert MemberName and Member JSONValue into value
          Variable memberVar{ member.GetMetaType(), memberPtr };
          value.emplace(member.GetName(), memberVar.Serialize());
        }
			}

			return value;
		}

		//! @brief Specialization of Default Serialize Function
    DECLARE_DEFAULT_SERIALIZER(bool);
    DECLARE_DEFAULT_SERIALIZER(int);
    DECLARE_DEFAULT_SERIALIZER(unsigned);
    DECLARE_DEFAULT_SERIALIZER(float);
    DECLARE_DEFAULT_SERIALIZER(double);
    DECLARE_DEFAULT_SERIALIZER(unsigned long long);
    DECLARE_DEFAULT_SERIALIZER(std::string);

    //GameObject
    template <>
    JsonValue DefaultSerializer<Core::ECS::GameObject&>(Reflection::Variable& variable);

    template <>
    JsonValue DefaultSerializer<Graphic::Material&>(Reflection::Variable& variable);

		//***********************************************************************
		// Deserialize
		//***********************************************************************

		//! @brief Default Deserialize Function
		template <typename T>
		inline void DefaultDeserializer(ValueObject& valueObject,Reflection::Variable& variable)
		{
			using namespace Reflection;
			//Value to hold all serialize data
			auto& members = variable.GetMetaType()->GetMembers();

			// Trying to Deserialize non-primitive Type 
			// with no member is an error
			ASSERT_TRUE(members.size() > 0);

			//Deserialize all the members
			for (auto& member : members)
			{
        if (member.GetMetaType()->ShouldSerialized()
          && member.ShouldSerialized())
        {
          //Actual Member data by offset from Object data pointer
          void* memberPtr = POINTER_OFFSET(variable.GetValue()
            , member.GetOffset());
          Variable memberVar{ member.GetMetaType()
            , memberPtr };

          //Find ValueObject Corresponding to member name
          auto& obj = valueObject.get_object();
          auto it = obj.find(member.GetName());
          if (it != obj.end())
          {
            //Deserialize ValueObject into Variable
            memberVar.Deserialize(it->second);
          }
          else
          {
            Debug::Log << Logger::MessageType::WARNING
              << "Not Found Deserialize MemberName: "
              << member.GetName() << '\n';
          }
        }
			}
		}

		//! @brief Specialization of Default Deserialize Function
    DECLARE_DEFAULT_DESERIALIZER(bool);
    DECLARE_DEFAULT_DESERIALIZER(int);
    DECLARE_DEFAULT_DESERIALIZER(unsigned);
    DECLARE_DEFAULT_DESERIALIZER(float);
    DECLARE_DEFAULT_DESERIALIZER(double);
    DECLARE_DEFAULT_DESERIALIZER(unsigned long long);
    DECLARE_DEFAULT_DESERIALIZER(std::string);

    //GameObject
    template <>
    void DefaultDeserializer<Core::ECS::GameObject&>(ValueObject& valueObject, Reflection::Variable& variable);

    template <>
    void DefaultDeserializer<Graphic::Material&>(ValueObject& valueObject, Reflection::Variable& variable);
	}
}