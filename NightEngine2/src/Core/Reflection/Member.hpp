/*!
  @file Member.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Member
*/
#pragma once
#include <string>

namespace Core
{
  namespace Reflection
  {
		class MetaType;

		/*!
		@brief Member class for POD struct
		*/
    class Member
    {
    public:
			//! @brief Useful for serializing derived class
			enum class AccessType : unsigned
			{
				PUBLIC = 0, PROTECTED, PRIVATE
			};

			//! @brief Constructor
      Member(const std::string& name, size_t offset, MetaType* metaType
			, AccessType type = AccessType::PUBLIC, bool shouldSeriazlie = true)
      : m_name(name), m_offset(offset), m_metaType(metaType)
			, m_accessType(type), m_shouldSerialized(shouldSeriazlie){}

			//! @brief Constructor for overriding access type
			Member(const Member& member, AccessType type)
				: Member(member.m_name, member.m_offset, member.m_metaType
          , type, member.m_shouldSerialized)
			{
				m_accessType = type;
			}

			//! @brief Getters
			const std::string& GetName(void) const{ return m_name; }
			size_t             GetOffset(void) const{ return m_offset; }
			MetaType*          GetMetaType(void) { return m_metaType;}
			AccessType         GetAccessType(void) const { return m_accessType; }

      //! @brief Should serialized or not
      bool ShouldSerialized(void) { return m_shouldSerialized; }

			//! @brief Log information to Debug::Log
			void LogInfo(void) const;
    private:
      std::string m_name;	//Member name
      size_t m_offset;		//Offset within Data

      MetaType* m_metaType;			//Type of member
			AccessType m_accessType;	//Access Type
      bool      m_shouldSerialized = false;
    };
  }
}