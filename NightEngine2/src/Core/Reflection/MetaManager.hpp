/*!
  @file MetaManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MetaManager
*/
#pragma once
#include "Core/Container/Map.hpp"
#include "Core/Container/String.hpp"
#include "Core/Container/PrimitiveType.hpp"

#include "Core/Reflection/MetaType.hpp"
#include "Core/Serialization/SerializeFunction.hpp"

namespace Core
{
  namespace Reflection
  {
		//Forward Declaration
    class MetaType;

		/*!
		@brief For Managing  MetaType
		*/
    class MetaManager
    {
      public:
        using MetaMap = Container::Map<Container::String, MetaType*>;
				
        static void Register(const Container::String& name, MetaType* metaType);
        static MetaType* Lookup(const Container::String& name);

				static MetaMap& GetMetaMap(void);

				/*! @brief Get an instance of MetaType */
				template<typename TYPE>
				static MetaType* GetMetaType(void)
				{
					static MetaType instance;
					return &instance;
				}

				/*! @brief Create and Register type into MetaType Map */
				template<typename TYPE>
				static void RegisterType(Container::String name, Container::U64 hash
        , size_t size, BaseClass baseClass
				, typename MetaType::SerializeFn serializeFn = nullptr
				, typename MetaType::DeserializeFn deserializeFn = nullptr
        , bool shouldSerialized = true)
				{
					MetaType* metaType = GetMetaType<TYPE>();

					//Init value and register itself to Manager
					(*metaType).Init(name, hash, size, baseClass
					, serializeFn == nullptr? Serialization::DefaultSerializer<TYPE>: serializeFn
					, deserializeFn == nullptr? Serialization::DefaultDeserializer<TYPE>: deserializeFn
          , shouldSerialized);

					Register(name, metaType);
				}

				/*! @brief Add member to a TYPE */
				template<typename TYPE, typename MEMBERTYPE>
				static void AddMember(const Container::String& name, MEMBERTYPE TYPE::*member
				, Member::AccessType accessType, bool shouldSerialized = true)
				{
					//TODO: Way to pass in Member AccessType
					size_t offset = (char*)&((TYPE*)nullptr->*member) - (char*)nullptr;
					MetaType* metaType = GetMetaType<TYPE>();

					metaType->AddMember(name, offset, GetMetaType<MEMBERTYPE>()
            , accessType, shouldSerialized);
				}
    };
  }
}