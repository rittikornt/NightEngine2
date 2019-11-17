/*!
  @file Variable.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Variable
*/
#include "Core/Reflection/Variable.hpp"

#include "Core/Reflection/MetaType.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

#include "Core/Macros.hpp"

namespace Core
{
  namespace Reflection
  {
		Variable::Variable(MetaType* metaType, void * data)
		: m_metaType(metaType), m_data(data) {}

		void Variable::SetVariable(MetaType* metaType, void * data)
		{
			m_metaType = metaType;
			m_data = data;
		}

		JsonValue Variable::Serialize(void)
		{
			ASSERT_TRUE(m_metaType != nullptr);

			return m_metaType->Serialize(*this);
		}

		void Variable::Deserialize(ValueObject & valueObject)
		{
			m_metaType->Deserialize(valueObject, *this);
		}
	}
}