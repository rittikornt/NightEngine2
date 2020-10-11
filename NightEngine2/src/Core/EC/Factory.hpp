/*!
  @file Factory.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Factory
*/
#pragma once
#include "Core/Macros.hpp"
#include "Core/EC/Handle.hpp"

#include "Core/Container/Hashmap.hpp"
#include "Core/Container/PrimitiveType.hpp"

//**********************************************
// Factory Helper Macros
//**********************************************
//! @brief Factory Registering Call (must be called within function)
#define FACTORY_REGISTER_TYPE(TYPE) \
	NightEngine::Factory::g_factory.Register(#TYPE, \
  NightEngine::Factory::HandleObjectFactory::InfoFN{FactoryCreate##TYPE,FactoryLookup##TYPE, FactoryDestroy##TYPE});																

//! @brief Factory Registering Call (must be called within function)
#define FACTORY_REGISTER_TYPE_WITHPARAM(TYPE, RESERVE_INT, EXPAND_RATE) \
	NightEngine::Factory::g_factory.Register(#TYPE, \
  NightEngine::Factory::HandleObjectFactory::InfoFN{FactoryCreate##TYPE,FactoryLookup##TYPE, FactoryDestroy##TYPE}); \
	NightEngine::Factory::GetTypeContainer<TYPE>().Reserve(RESERVE_INT, EXPAND_RATE); 	

//! @brief Factory Lookup/Destroy/Create Functions Implementation
#define FACTORY_FUNC_IMPLEMENTATION(TYPE) \
	void* FactoryLookup##TYPE(const NightEngine::Container::SlotmapID& id) \
	{	\
		return NightEngine::Factory::GetTypeContainer<TYPE>().Get(id); \
	}	\
	void FactoryDestroy##TYPE(const NightEngine::Container::SlotmapID& id)	\
	{	\
		NightEngine::Factory::GetTypeContainer<TYPE>().Destroy(id);	\
	}	\
	NightEngine::EC::HandleObject FactoryCreate##TYPE()	\
	{	\
		return NightEngine::EC::HandleObject(NightEngine::Factory::GetTypeContainer<TYPE>().CreateSlot()	\
					, FactoryLookup##TYPE, FactoryDestroy##TYPE);	 \
	}																		 

//! @brief Cast SlowMapID to HandleObject of specific TYPE
#define CAST_SLOTMAPID_TO_HANDLEOBJECT(TYPE, SLOTMAPID) \
  NightEngine::Factory::Cast<TYPE>(#TYPE, SLOTMAPID);

namespace NightEngine
{
	using namespace Container;

  namespace Factory
  {
    //! @brief Factory initialization
		void Initialize(void);

    //! @brief Factory Termination
    void Terminate(void);

    //! @brief Create Object from typeName
    EC::HandleObject Create(const char* typeName);

    //! @brief Create Object from typeName
		template<class T>
    EC::Handle<T> Create(const char* typeName);

    //! @brief Get the slotmap that internally store object of type T
		template<class T>
		Slotmap<T>& GetTypeContainer();

    //**********************************************
    // Factory Class
    //**********************************************
    class HandleObjectFactory
    {
		public:
      using CreateFN = EC::HandleObject (*)(void);
      struct InfoFN
      {
        CreateFN m_createFn;
        EC::HandleObject::LookupFN m_lookupFn;
        EC::HandleObject::DestroyFN m_destroyFn;
      };

			//! @brief Register typename into Factory
      void	            Register(const char* name, InfoFN infoFn);

      //! @brief Create object from typename
      EC::HandleObject	Create(const char* typeName);

      //! @brief Get function info for specific type
      InfoFN	          GetFunctionInfo(const char* typeName);

      void              Clear(void) { m_handleMap.clear(); }
		private:

			using HandleInfoMap = Container::Hashmap<std::string, InfoFN>;
      HandleInfoMap m_handleMap;	//Map of name to Creator
		};

		//Global Variable
		extern HandleObjectFactory g_factory;
	}
}

//**********************************************
// Template Definitions
//**********************************************
namespace NightEngine
{
  namespace Factory
  {
    //**********************************************
    // Class Definition
    //**********************************************
    inline void HandleObjectFactory::Register(const char* name, InfoFN infoFn)
    {
      m_handleMap.insert( {name, infoFn} );
    }

    inline EC::HandleObject HandleObjectFactory::Create(const char* typeName)
    {
      ASSERT_MSG(m_handleMap.find(typeName) != m_handleMap.end()
        , "Trying to Create an unregistered typeName");
      return m_handleMap[typeName].m_createFn();
    }

    inline HandleObjectFactory::HandleObjectFactory::InfoFN HandleObjectFactory::GetFunctionInfo(const char* typeName)
    {
      ASSERT_MSG(m_handleMap.find(typeName) != m_handleMap.end()
        , "Trying to Lookup an unregistered typeName");
      return m_handleMap[typeName];
    }

    //**********************************************
    // Functions Definition
    //**********************************************
    template<class T>
    EC::Handle<T> Create(const char* typeName)
    {
      return EC::Handle<T>(Create(typeName));
    }

    template<class T>
    Slotmap<T>& GetTypeContainer()
    {
      static Slotmap<T> slotmap(10, 5);
      return slotmap;
    }

    template<class T>
    EC::Handle<T> Cast(const char* typeName, SlotmapID slotmapID)
    {
      auto funcInfo = g_factory.GetFunctionInfo(typeName);
      return EC::Handle<T>
      {
        EC::HandleObject{ slotmapID, funcInfo.m_lookupFn, funcInfo.m_destroyFn }
      };
    }
  }
}