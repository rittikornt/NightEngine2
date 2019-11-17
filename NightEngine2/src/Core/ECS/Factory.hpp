/*!
  @file Factory.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Factory
*/
#pragma once
#include "Core/Macros.hpp"
#include "Core/ECS/Creator.hpp"
#include "Core/ECS/Handle.hpp"

#include "Core/Container/Hashmap.hpp"
#include "Core/Container/PrimitiveType.hpp"

namespace Core
{
	using namespace Container;

  namespace Factory
  {
    //! @brief Factory initialization
		void Initialize(void);

    //! @brief Factory Termination
    void Terminate(void);

    //! @brief Create Object from typeName
		HandleObject Create(const char* typeName);

    //! @brief Create Object from typeName
		template<class T>
		Handle<T> Create(const char* typeName);

    //! @brief Get the slotmap that internally store object of type T
		template<class T>
		Slotmap<T>& GetTypeContainer();

    //**********************************************
    // Factory Class
    //**********************************************

		//Factory Declaration
		template<class T>
    class ObjectFactory
    {
		public:
			//! @brief Register Creator<T> into Factory
			template<class Type>
			void	 Register(Creator<T>* creator);

      //! @brief Create object from typename
			T			 Create(const char* typeName);
		private:
			using CreatorMap = Container::Hashmap<std::string, Creator<T>>;
			CreatorMap m_creatorMap;	//Map of name to Creator
		};

		//Global Variable
		extern ObjectFactory<HandleObject> g_factory;

		//**********************************************
		// Class Definition
    //**********************************************

		template<class T>
		template<class Type>
		inline void ObjectFactory<T>::Register(Creator<T>* creator)
		{
			m_creatorMap.insert({ creator->m_name, std::move(*creator) });
		}
		
		template<class T>
		inline T ObjectFactory<T>::Create(const char * typeName)
		{
			ASSERT_MSG(m_creatorMap.find(typeName) != m_creatorMap.end()
          ,"Trying to Create an unregistered type T");
			return m_creatorMap[typeName].m_createFN();
		}

    //**********************************************
    // Functions Definition
    //**********************************************
		template<class T>
		Handle<T> Create(const char* typeName)
		{
			return Handle<T>(Create(typeName));
		}

		template<class T>
		Slotmap<T>& GetTypeContainer()
		{
			static Slotmap<T> slotmap(10,5);
			return slotmap;
		}
	}
}