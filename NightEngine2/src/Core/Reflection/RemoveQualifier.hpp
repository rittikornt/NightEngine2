/*!
  @file RemoveQualifier.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RemoveQualifier
*/
#pragma once

namespace NightEngine
{
  namespace Reflection
  {
		/*!
		@brief Class for recursively removing the qualifier: const, &, &&
		*/
    template<typename T>
    struct RemoveQualifier
    {
      using RawType = T;
    };

    //Using Template Specialization
    template<typename T>
    struct RemoveQualifier<const T>
    {
      using RawType = typename RemoveQualifier<T>::RawType;
    };

    template<typename T>
    struct RemoveQualifier<T&>
    {
      using RawType = typename RemoveQualifier<T>::RawType;
    };

    template<typename T>
    struct RemoveQualifier<T&&>
    {
      using RawType = typename RemoveQualifier<T>::RawType;
    };
    
		//! @brief Specialization for const T* is T*
    template<typename T>
    struct RemoveQualifier<const T *>
    {
      using RawType = typename RemoveQualifier<T*>::RawType;
    };
  }
}

/*!
@brief Alias for shorter use
*/
template<typename T>
using RawType = typename NightEngine::Reflection::RemoveQualifier<T>::RawType;