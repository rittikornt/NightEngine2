/*!
  @file MetaManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MetaManager
*/
#pragma once
#include "Core/Container/Map.hpp"
#include "Core/Container/Vector.hpp"
#include "Core/Container/String.hpp"
#include "Core/Container/PrimitiveType.hpp"

#include "Core/Reflection/MetaType.hpp"
#include "Core/Serialization/SerializeFunction.hpp"

namespace NightEngine
{
  namespace Reflection
  {
		//Forward Declaration
    class MetaType;

		/*!
		@brief Static class that manage MetaType
		*/
    class MetaManager
    {
      public:
        using MetaMap = Container::Map<Container::String, MetaType*>;
				
        static void Register(const Container::String& name, MetaType* metaType);
        static MetaType* Lookup(const Container::String& name);

        ///////////////////////////////////////////////////////////////

        /*! @brief Get global instance of MetaMap */
				static MetaMap& GetMetaMap(void)
        {
          static MetaMap map;
          return map;
        }

				/*! @brief Get global instance of a specific MetaType */
				template<typename TYPE>
				static MetaType* GetMetaType(void)
				{
					static MetaType instance;
					return &instance;
				}

        ///////////////////////////////////////////////////////////////

				/*! @brief Create and Register type into MetaType Map */
				template<typename TYPE>
				static void RegisterType(Container::String typeName, Container::U64 hash
        , size_t size, BaseClass baseClass
				, typename MetaType::SerializeFn serializeFn = nullptr
				, typename MetaType::DeserializeFn deserializeFn = nullptr
        , bool shouldSerialized = true)
				{
					MetaType* metaType = GetMetaType<TYPE>();

					//Init value and register itself to Manager
					(*metaType).Init(typeName, hash, size, baseClass
					, serializeFn == nullptr? Serialization::DefaultSerializer<TYPE>: serializeFn
					, deserializeFn == nullptr? Serialization::DefaultDeserializer<TYPE>: deserializeFn
          , shouldSerialized);

					Register(typeName, metaType);
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

    ///////////////////////////////////////////////////////////////

    /*!
    @brief Global class for storing function pointer to be called to init some metatype
    */
    struct ReflectionInitFunctions
    {
      using ReflectionInitFn = void(*)(void);
      static NightEngine::Container::Vector<ReflectionInitFn>& GetFunctions()
      {
        static NightEngine::Container::Vector<ReflectionInitFn> funcs;
        return funcs;
      }

      static void Add(ReflectionInitFn func)
      {
        auto& funcs = GetFunctions();
        funcs.emplace_back(func);
      }

      static void InvokeAll()
      {
        //TODO: Care about initialization precedence
        auto& funcs = GetFunctions();
        for (int i=0; i < funcs.size(); ++i)
        {
          funcs[i]();
        }
      }
    };

    /*!
    @brief Class for registering reflection initialization functions
    */
    template<typename TYPE>
    struct ReflectionInitFunctionsRegisterer
    {
      ReflectionInitFunctionsRegisterer()
      {
        ReflectionInitFunctions::Add(&TYPE::ReflectionInit);
      }

      ReflectionInitFunctionsRegisterer(ReflectionInitFunctions::ReflectionInitFn func)
      {
        ReflectionInitFunctions::Add(&TYPE::ReflectionInit);
        ReflectionInitFunctions::Add(func);
      }
    };

    /*!
    @brief Class for registering metatype 
    */
    template<typename TYPE>
    class MetaRegisterer
    {
    public:
      MetaRegisterer<TYPE> RegisterType(Container::String typeName, Container::U64 hash
        , BaseClass baseClass
        , typename MetaType::SerializeFn serializeFn = nullptr
        , typename MetaType::DeserializeFn deserializeFn = nullptr
        , bool shouldSerialized = true)
      {
        ReflectionManager::RegisterType<RawType<TYPE>>(typeName, hash
          , sizeof(TYPE), baseClass
          , serializeFn, deserializeFn, shouldSerialized);

        //ReflectionInitFunctions::Add(TYPE::ReflectionInit);
        return *this;
      }

      template<typename MEMBERTYPE>
      MetaRegisterer<TYPE> AddMember(const Container::String& memberTypeName, MEMBERTYPE TYPE::*member
        , Member::AccessType accessType, bool shouldSerialized = true)
      {
        ReflectionManager::AddMember<RawType<TYPE>>(memberTypeName, member
          , accessType, shouldSerialized);
        return *this;
      }
    };
  }
}