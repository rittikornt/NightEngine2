/*!
  @file ComponentLogic.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ComponentLogic
*/
#include "Core/ECS/ComponentLogic.hpp"

namespace Core
{
	namespace ECS
	{
		ComponentLogicID ComponentLogic::s_uniqueIDCounter = 0;

		ComponentHandle::ComponentHandle(GameObject* gameObject
			, Factory::HandleObject handle, Reflection::MetaType* metaType)
			: m_handle(handle), m_metaType(metaType)
		{
      auto cl = Get<ComponentLogic>();
      cl->m_gameObject = gameObject;
      cl->m_handle = handle;
		}
	}
}