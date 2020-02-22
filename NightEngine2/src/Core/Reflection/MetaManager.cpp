/*!
  @file MetaManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MetaManager
*/
#include "Core/Reflection/MetaManager.hpp"

namespace NightEngine
{
  namespace Reflection
  {
		/*!
		@brief Register MetaType into a map
		*/
		void MetaManager::Register(const Container::String& name, MetaType* metaType)
		{
			GetMetaMap()[name] = metaType;
		}

		/*!
		@brief Lookup MetaType from a map
		*/
		MetaType* MetaManager::Lookup(const Container::String& name)
		{
			auto it = GetMetaMap().find(name);

      //Trying to Access unregistered MetaType is an error
      ASSERT_MSG(it != GetMetaMap().end(), "Trying to lookup unregistered MetaType");
      ASSERT_MSG(it->second->GetName() != "", "Trying to lookup unregistered MetaType");

			//Make sure this return nullptr if not found
			return it->second;
		}

	}
}