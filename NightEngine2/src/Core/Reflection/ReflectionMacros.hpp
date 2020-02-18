/*!
  @file ReflectionMacros.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ReflectionMacros
*/
#pragma once

//Need to fix Window header Macros name conflict with taocpp Enum
#ifdef OPTIONAL
  #undef OPTIONAL
#endif

#include "Core/Reflection/ReflectionCore.hpp"
#include "Core/Reflection/MetaManager.hpp"
#include "Core/Reflection/RemoveQualifier.hpp"

#include "Core/Macros.hpp"
#include "Core/Container/MurmurHash2.hpp"

using ReflectionManager = Core::Reflection::MetaManager;

using InheritType = Core::Reflection::BaseClass::InheritType;
using AccessType = Core::Reflection::Member::AccessType;

//************************************************************
// Register Type Within a ReflectionCore.cpp Initialization
//************************************************************

//! @brief Register type to Reflection System
#define REGISTER_METATYPE(TYPE) \
ReflectionManager::RegisterType<RawType<TYPE>>(#TYPE, MURMURHASH2(TYPE)\
,sizeof(TYPE)\
,{ nullptr, InheritType::PUBLIC})

//! @brief Register type to Reflection System with specific name
#define REGISTER_METATYPE_WITHNAME(TYPE, NAME) \
ReflectionManager::RegisterType<RawType<TYPE>>(NAME, MURMURHASH2(TYPE)\
,sizeof(TYPE)\
,{ nullptr, InheritType::PUBLIC})

//! @brief Register type to Reflection System with specific base class and inherit type
#define REGISTER_METATYPE_WITHBASE(TYPE, BASETYPE, INHERITTYPE) \
ReflectionManager::RegisterType<RawType<TYPE>>(#TYPE, MURMURHASH2(TYPE)\
,sizeof(TYPE), \
{ReflectionManager::GetMetaType<RawType<BASETYPE>>() \
, INHERITTYPE} )

#define REGISTER_METATYPE_WITH_SERIALIZER(TYPE, SHOULDSERIALIZED,SERIALIZER, DESERIALIZER) \
ReflectionManager::RegisterType<RawType<TYPE>>(#TYPE, MURMURHASH2(TYPE)\
,sizeof(TYPE)\
,{ nullptr, InheritType::PUBLIC}, SERIALIZER, DESERIALIZER, SHOULDSERIALIZED)

//************************************************************
// Add member to the registered METATYPE
//************************************************************

//! @brief Register member to type with specific accesstype
#define ADD_MEMBER(TYPE, MEMBER, ACCESSTYPE) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, ACCESSTYPE)

//! @brief This is required for MSVC to expand __VA_ARGS__ into multiple arguments
#define EXPAND( x ) x

//! @brief Choose the right Macros function
#define ADD_MEMBER_ARG_CHOOSER(ARG1, ARG2, ARG3, FUNC, ...) FUNC

/////////////////////////////////////////////////

//! @brief Register member to type with specific public access
#define ADD_MEMBER_PUBLIC_ARG_2(TYPE, MEMBER ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PUBLIC)

//! @brief Register member to type with specific public access
#define ADD_MEMBER_PUBLIC_ARG_3(TYPE, MEMBER, SHOULDSERIALIZED ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PUBLIC, SHOULDSERIALIZED)

//! @brief Register member to type with specific public access
#define ADD_MEMBER_PUBLIC(...) \
  EXPAND( ADD_MEMBER_ARG_CHOOSER(__VA_ARGS__ \
          , ADD_MEMBER_PUBLIC_ARG_3 \
          , ADD_MEMBER_PUBLIC_ARG_2)(__VA_ARGS__) )

/////////////////////////////////////////////////

//! @brief Register member to type with specific private access
#define ADD_MEMBER_PRIVATE_ARG_2(TYPE, MEMBER ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PRIVATE)

//! @brief Register member to type with specific private access
#define ADD_MEMBER_PRIVATE_ARG_3(TYPE, MEMBER, SHOULDSERIALIZED ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PRIVATE, SHOULDSERIALIZED)

//! @brief Register member to type with specific private access
#define ADD_MEMBER_PRIVATE(...) \
  EXPAND( ADD_MEMBER_ARG_CHOOSER(__VA_ARGS__ \
          , ADD_MEMBER_PRIVATE_ARG_3 \
          , ADD_MEMBER_PRIVATE_ARG_2)(__VA_ARGS__) )

/////////////////////////////////////////////////

//! @brief Register member to type with specific protected access
#define ADD_MEMBER_PROTECED_ARG_2(TYPE, MEMBER ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PROTECTED)

//! @brief Register member to type with specific protected access
#define ADD_MEMBER_PROTECED_ARG_3(TYPE, MEMBER, SHOULDSERIALIZED  ) \
	ReflectionManager::AddMember<RawType<TYPE>>( #MEMBER, &TYPE::MEMBER \
, AccessType::PROTECTED, SHOULDSERIALIZED)

//! @brief Register member to type with specific protected access
#define ADD_MEMBER_PROTECED(...) \
  EXPAND( ADD_MEMBER_ARG_CHOOSER(__VA_ARGS__ \
          , ADD_MEMBER_PROTECED_ARG_3 \
          , ADD_MEMBER_PROTECED_ARG_2)(__VA_ARGS__) )

//************************************************************
// Flag embedded within a class for private access
//************************************************************

//! @brief Give reflection system access to a class/struct private/protected section
#define REFLECTABLE_TYPE() \
		friend void Core::Reflection::Initialize(void); \
    template <typename T> \
    friend Core::JsonValue Core::Serialization::DefaultSerializer(Core::Reflection::Variable&); \
    template <typename T> \
    friend void Core::Serialization::DefaultDeserializer(Core::ValueObject&,Core::Reflection::Variable&);

//! @brief Give reflection system access to a class/struct private/protected section
#define REFLECTABLE_TYPE_BLOCK() \
    public: \
		friend void Core::Reflection::Initialize(void); \
    template <typename T> \
    friend Core::JsonValue Core::Serialization::DefaultSerializer(Core::Reflection::Variable&); \
    template <typename T> \
    friend void Core::Serialization::DefaultDeserializer(Core::ValueObject&,Core::Reflection::Variable&); \
    static void ReflectionInit()

//! @brief Register reflection init function to the global class to be called at init time, must be in .cpp file
#define INIT_REFLECTION_FOR(TYPE) \
    static Core::Reflection::ReflectionInitFunctionsRegisterer<TYPE> g_registerer##TYPE; \
//************************************************************
// Getter Macros
//************************************************************

//! @brief Get MetaType by type
#define METATYPE(TYPE) (ReflectionManager::GetMetaType<RawType<TYPE> >())

//! @brief Get MetaType by object instance
#define METATYPE_FROM_OBJECT(OBJECT) (ReflectionManager::GetMetaType<RawType<decltype(OBJECT)> >())

//! @brief Get MetaType by string
#define METATYPE_FROM_STRING(STR) (ReflectionManager::Lookup(STR))

//************************************************************
// Log Utility
//************************************************************

//! @brief Log information about MetaType into Debug::Log
#define LOGINFO_METATYPE(TYPE) (METATYPE(TYPE)->LogInfo())

//************************************************************
// META Registerer Macros
//************************************************************
#define META_REGISTERER(TYPE, SHOULDSERIALIZED,SERIALIZER, DESERIALIZER) static Core::Reflection::MetaRegisterer<TYPE> registerer##TYPE \
        = Core::Reflection::MetaRegisterer<TYPE>() \
          .RegisterType(#TYPE, MURMURHASH2(TYPE) \
            , sizeof(TYPE), { nullptr, InheritType::PUBLIC} \
            , SERIALIZER, DESERIALIZER, SHOULDSERIALIZED)

#define META_REGISTERER_WITHNAME(TYPE, NEWNAME, SHOULDSERIALIZED,SERIALIZER, DESERIALIZER) static Core::Reflection::MetaRegisterer<TYPE> registerer##TYPE \
        = Core::Reflection::MetaRegisterer<TYPE>() \
          .RegisterType(NEWNAME, MURMURHASH2(TYPE) \
            , sizeof(TYPE), { nullptr, InheritType::PUBLIC} \
            , SERIALIZER, DESERIALIZER, SHOULDSERIALIZED)

#define META_REGISTERER_WITHBASE(TYPE, BASETYPE, INHERITTYPE, SHOULDSERIALIZED,SERIALIZER, DESERIALIZER) static Core::Reflection::MetaRegisterer<TYPE> registerer##TYPE \
        = Core::Reflection::MetaRegisterer<TYPE>() \
          .RegisterType(#TYPE, MURMURHASH2(TYPE) \
            , sizeof(TYPE), { ReflectionManager::GetMetaType<RawType<BASETYPE>>(), INHERITTYPE} \
            , SERIALIZER, DESERIALIZER, SHOULDSERIALIZED)

/////////////////////////////////////////////////

//! @brief Register member to type with specific public access
#define MR_ADD_MEMBER_PUBLIC(TYPE, MEMBERNAME, SHOULDSERIALIZED ) \
          AddMember(#MEMBERNAME, &TYPE::MEMBERNAME \
          , AccessType::PUBLIC, SHOULDSERIALIZED)

//! @brief Register member to type with specific private access
#define MR_ADD_MEMBER_PRIVATE(TYPE, MEMBERNAME, SHOULDSERIALIZED ) \
          AddMember(#MEMBERNAME, &TYPE::MEMBERNAME \
          , AccessType::PRIVATE, SHOULDSERIALIZED)

//! @brief Register member to type with specific protected access
#define MR_ADD_MEMBER_PROTECTED(TYPE, MEMBERNAME, SHOULDSERIALIZED ) \
          AddMember(#MEMBERNAME, &TYPE::MEMBERNAME \
          , AccessType::PROTECTED, SHOULDSERIALIZED)