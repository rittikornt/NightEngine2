/*!
  @file Variable.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Variable
*/

#pragma once
#include <ostream>

//JsonValue
#include "Core/Serialization/taocpp_json/include/tao/json/value.hpp"
#include "Core/Serialization/taocpp_json/include/tao/json/basic_value.hpp"

//! @brief Macros for help access member within a Data Object (work with private too)
#define POINTER_OFFSET(VOIDPTR, OFFSET)\
	(reinterpret_cast<void*>(reinterpret_cast<char*>(VOIDPTR) + (OFFSET) ))

namespace Core
{
	using JsonValue = tao::json::value;
	using ValueObject = tao::json::basic_value<tao::json::traits>;
  namespace Reflection
  {
    //Forward declaration
    class MetaType;

    //! @brief Class for referencing any variable, useful for serialization
    class Variable
    {
      public:
      //! @brief Variable Constructor
			Variable(MetaType* metaType,void* data);

      //! @brief Get the Internal Dereferenced Data 
      template<typename T>
      T& GetValue(void)
      {
        return *(reinterpret_cast<T*>(m_data));
      }
			//! @brief Get raw pointer to data 
			void* GetValue(void) { return m_data; }

			//! @brief Get Pointer to MetaType
			MetaType* GetMetaType(void) { return m_metaType; }

			//! @brief Set value to internal Data 
			template<typename T>
			void SetValue(T& value)
			{
				*(reinterpret_cast<T*>(m_data))= value;
			}

      //! @brief Set Variable
			void SetVariable(MetaType* metaType, void* data);

			//! @brief Serialize this variable into JsonValue
			JsonValue Serialize(void);

			//! @brief Deserialize valueObject into this Variable
			void Deserialize(ValueObject& valueObject);
      private:
      MetaType* m_metaType;
      void* m_data;
    };
  }
}