/*!
  @file Member.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Member
*/
#include "Core/Reflection/Member.hpp"
#include "Core/Logger.hpp"

#include "Core/Reflection/MetaType.hpp"

namespace Core
{
	namespace Reflection
	{
		void Member::LogInfo(void) const
		{
			Debug::Log << m_metaType->GetName() << " " << m_name 
				<< ", Size: " << m_metaType->GetSize() << ", Offset: " << m_offset  << '\n';
		}
	}
}