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
	NightEngine::Factory::g_factory.Register<TYPE>(#TYPE, FactoryCreate##TYPE);																

//! @brief Factory Registering Call (must be called within function)
#define FACTORY_REGISTER_TYPE_WITHPARAM(TYPE, RESERVE_INT, EXPAND_RATE) \
	NightEngine::Factory::g_factory.Register<TYPE>(#TYPE, FactoryCreate##TYPE); \
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
		template<class T>
    class ObjectFactory
    {
		public:
      using CreateFN = T(*)(void);

			//! @brief Register Creator<T> into Factory
			template<class Type>
      void	 Register(const char* name, CreateFN createFn);//Creator<T>* creator);

      //! @brief Create object from typename
			T			 Create(const char* typeName);
		private:

			using TypeCreateFnMap = Container::Hashmap<std::string, CreateFN>;
			TypeCreateFnMap m_creatorMap;	//Map of name to Creator
		};

		//Global Variable
		extern ObjectFactory<EC::HandleObject> g_factory;
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
    template<class T>
    template<class Type>
    inline void ObjectFactory<T>::Register(const char* name, CreateFN createFn)
    {
      m_creatorMap.insert({ name, createFn });
    }

    template<class T>
    inline T ObjectFactory<T>::Create(const char * typeName)
    {
      ASSERT_MSG(m_creatorMap.find(typeName) != m_creatorMap.end()
        , "Trying to Create an unregistered type T");
      return m_creatorMap[typeName]();
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
  }
}