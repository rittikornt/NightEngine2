/*!
  @file MetaType.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MetaType
*/
#include "Core/Reflection/MetaType.hpp"

#include "Core/Logger.hpp"
#include "Core/Macros.hpp"
#include "Core/Reflection/Variable.hpp"

namespace Core
{
	namespace Reflection
	{
    using namespace Container;

		//! @brief Initializer should only be called in MetaManager
		void MetaType::Init(const std::string & name, U64 hash, size_t size
			, BaseClass& base, SerializeFn serializeFn, DeserializeFn deserializeFn
      , bool shouldSerialized)
		{
      //Debug::Log << "MetaType: " << name << ", Hash: " << hash << '\n';
			m_name = name;
      m_hash = hash;
      m_size = size;
			m_baseClass = base;

			m_serializeFn = serializeFn;
			m_deserializeFn = deserializeFn;
      m_shouldSerialized = shouldSerialized;

			//Try to inherit member from the base class
			if (m_baseClass.m_metaType != nullptr)
			{
				//For all member in base class
				auto& baseClassMembers = m_baseClass.m_metaType->GetMembers();
				for (auto& member : baseClassMembers)
				{
					//Inherit only public and protected access member
					if (member.GetAccessType() != Member::AccessType::PRIVATE)
					{
						//Public Inheritance: Map public/protected directly
						//Protected Inheritance: Map public/protected to protected
						//Private Inheritance: Map public/protected to private
						auto inheritType = base.m_inheritType == BaseClass::InheritType::PUBLIC ?
							member.GetAccessType()
							: (Member::AccessType)(static_cast<unsigned>(base.m_inheritType));
						m_members.emplace_back(member, inheritType);
					}
				}
			}
		}

		//! @brief Add Member to the Type
		void MetaType::AddMember(const std::string & name, size_t offset
			, MetaType * memberType, Member::AccessType accessType, bool shouldSerialized)
		{
			m_members.emplace_back(name, offset, memberType
        , accessType, shouldSerialized);
		}

		//! @brief Find member by name in m_members
		Member* MetaType::FindMember(std::string name)
		{
			//Linearly looking for member
			for (auto& member : m_members)
			{
				if (member.GetName() == name)
				{
					return &member;
				}
			}
			return nullptr;
		}

		//! @brief Check if this Type is derived from baseClass
		bool MetaType::IsDerivedFrom(const MetaType* baseClass) const
		{
			const MetaType* meta = this;

			//Traverse all m_baseClass of this MetaType
			while (meta != nullptr)
			{
				//Look for baseClass
				if (meta == baseClass)
				{
					return true;
				}
				meta = meta->m_baseClass.m_metaType;
			}

			return false;
		}

		//! @brief Pretty Log to the Debug::Log
		void MetaType::LogInfo(void) const
		{
			Debug::Log << "MetaType(" << m_name;
			if (m_baseClass.m_metaType != nullptr)
			{
				Debug::Log << ": " << m_baseClass.m_metaType->GetName();
			}
			Debug::Log << "), Size: " << m_size << '\n';

			if (m_members.size() > 0)
			{
				Debug::Log << "{\n";
				for (auto& member : m_members)
				{
					Debug::Log << "  ";
					member.LogInfo();
				}
				Debug::Log << "}\n";
			}
		}

		//! @brief Function for Serialization
		JsonValue MetaType::Serialize(Variable& variable)
		{
			ASSERT_TRUE(m_serializeFn != nullptr);
			return m_serializeFn(variable);
		}

		//! @brief Function for Deserialization
		void MetaType::Deserialize(ValueObject & valueObject, Variable & variable)
		{
			ASSERT_TRUE(m_deserializeFn != nullptr);
			m_deserializeFn(valueObject, variable);
		}
	}
}