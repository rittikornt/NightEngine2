/*!
  @file Creator.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Creator
*/
#pragma once

namespace NightEngine
{
  namespace Factory
  {
		//Type for storing CreateFunction for factory
		template<typename CreateType>
		struct Creator
		{
			using CreateFN = CreateType (*)(void);

			Creator() = default;
			explicit Creator(CreateFN createFN, const char* name)
				: m_createFN(createFN), m_name(name) {}

			CreateFN m_createFN;
			const char* m_name;
		};
	}
}