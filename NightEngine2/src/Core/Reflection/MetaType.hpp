/*!
  @file MetaType.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MetaType
*/
#pragma once
#include <string>
#include "Core/Container/Vector.hpp"
#include "Core/Container/PrimitiveType.hpp"

#include "Core/Reflection/Member.hpp"

//JsonValue
#include "taocpp_json/include/tao/json/value.hpp"
#include "taocpp_json/include/tao/json/basic_value.hpp"

namespace NightEngine
{
	using JsonValue = tao::json::value;
	using ValueObject = tao::json::basic_value<tao::json::traits>;
  namespace Reflection
  {
		//Forward Declaration
		class Member;
		class Variable;
		class MetaType;

		//! @brief Light wrapper of MetaType* and InheritType
		struct BaseClass
		{
			enum class InheritType : unsigned
			{
				PUBLIC = 0, PROTECTED, PRIVATE
			};
			MetaType* m_metaType;	//Can modify to support multiple inheritance here
			InheritType m_inheritType;
		};

    //! @brief Class containing information for each type
    class MetaType
    {
    public:
			using SerializeFn = JsonValue(*)(Variable&);
			using DeserializeFn = void(*)(ValueObject&, Variable&);
			
      //! @brief Constructor
      MetaType(void) : m_name("Unregistered")
        , m_hash(0), m_size(0) {}

      //! @brief Intialization
			void Init(const std::string& name, Container::U64 hash, size_t size
				, BaseClass& base, SerializeFn serializeFn, DeserializeFn deserializeFn
        , bool shouldSerialized = true);
			
      //! @brief Add member to the type
      void AddMember(const std::string& name, size_t offset
				, MetaType* memberType, Member::AccessType accessType
        , bool shouldSerialized);
      
      //! @brief Get name of the type
			const std::string& 					GetName(void) const{ return m_name; }
      
      //! @brief Get Hash value of the name of the type
      Container::U64 							GetHash(void) const { return m_hash; }
			
      //! @brief Get Size of the type
      size_t 											GetSize(void) const{ return m_size; }
			
      //! @brief Get all the Members of this type
      Container::Vector<Member>& 	GetMembers(void) { return m_members; }

      //! @brief Should serialized or not
      bool ShouldSerialized(void) { return m_shouldSerialized; }

      //! @brief Find the member by name
			Member* FindMember(std::string name);

      //! @brief Check if this type is a subclass of baseClass or not
			bool IsDerivedFrom(const MetaType* baseClass) const;

      //! @brief Check if this type has a base class or not
			bool HasBaseClass(void) const { return m_baseClass.m_metaType != nullptr; }
			
      //! @brief Get the baseclass of this type
      BaseClass& GetBaseClass(void) { return m_baseClass; }

      //! @brief Debug log the information of this type
			void LogInfo(void) const;

      //! @brief Serialize this type with the value in variable
			JsonValue Serialize(Variable& variable);

      //! @brief Deserialize valueObject into variable
			void Deserialize(ValueObject& valueObject,Variable& variable);
    private:
      std::string m_name;
      Container::U64 m_hash;	//Hash of the m_name
      size_t m_size;

			BaseClass m_baseClass;	//Not support multiple inheritance
      Container::Vector<Member> m_members;

      bool m_shouldSerialized = true;
			SerializeFn m_serializeFn;
			DeserializeFn m_deserializeFn;
    };
  }

}