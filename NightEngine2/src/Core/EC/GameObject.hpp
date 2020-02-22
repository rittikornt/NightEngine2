/*!
  @file GameObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GameObject
*/
#pragma once
#include "Core/EC/Components/Transform.hpp"  //m_transform;
#include "Core/Message/IMessageHandler.hpp"
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Factory.hpp"
#include "Core/EC/Handle.hpp"

#include "Core/Container/String.hpp"
#include "Core/Container/Vector.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

//Forward declaration
namespace NightEngine
{
	class IMessageHandler;
	class PlayerUpdateMessage;
}

namespace NightEngine
{
	namespace EC
	{
    //! @brief GameObject Class
		class GameObject : public NightEngine::IMessageHandler
		{
      REFLECTABLE_TYPE();
		public:
      //! @brief HandleMessage Override
			virtual void HandleMessage(const NightEngine::PlayerUpdateMessage &msg);

      //! @brief Constructor
			GameObject();

      //! @brief Constructor
			GameObject(const char* name, size_t reserveSize);

      //! @brief Destructor
      ~GameObject();

      //! @brief Create GameObject from Factory
      static Handle<GameObject> Create(const char* name, size_t reserveSize);

      //! @brief Get GameObject Handle
      Handle<GameObject> GetHandle(void) const { return m_handle; }

      //! @brief Initialize the GameObject
      void Init(void);

      //! @brief Destroy the GameObject
      void Destroy(void);

      //! @brief Set the GameObject Name
			void	SetName(const Container::String& name) { m_name = name; }
      
      //! @brief Get GameObject Name
			const Container::String& GetName() const { return m_name; }

      //! @brief Get Pointer to Transform
      Components::Transform* GetTransform() const { return m_transform.Get(); }

      //! @brief Get Component Count
      size_t GetComponentCount() const { return m_components.size(); }

      //! @brief Get All Components
      Container::Vector<ComponentHandle>&		GetAllComponents(void);

      //! @brief GetComponent by Type
			template<class T>
			ComponentHandle*		GetComponent(void);

      //! @brief GetComponent by Name
			ComponentHandle*		GetComponent(const char*);

      //! @brief AddComponent by Name
			ComponentHandle*		AddComponent(const char*);

      //! @brief RemoveComponent by Name
      void		RemoveComponent(const char*);

      //! @brief RemoveComponent by Type
			template<class T>
			void								RemoveComponent();

      //! @brief Remove all components
      void								RemoveAllComponents();
		private:
			Container::String                      m_name;
      Handle<GameObject>            m_handle;
			Container::Vector<ComponentHandle>     m_components;
      Handle<Components::Transform> m_transform;
		};

    //********************************************
    // Definition
    //********************************************
		template<class T>
		inline ComponentHandle* GameObject::GetComponent()
		{
      using namespace Reflection;
      MetaType* metaType = METATYPE(T);

			//Linear Search for Component
			for (auto& handle : m_components)
			{
				//If component match the type
				if (handle.m_metaType == metaType)
				{
					return &handle;
				}
			}

			return nullptr;
		}
		
		template<class T>
		inline void GameObject::RemoveComponent()
		{
      using namespace Reflection;
      MetaType* metaType = METATYPE(T);

			//Linear Search for Component
			for (auto it = m_components.begin() 
				; it != m_components.end(); ++it)
			{
				//If component match the type
				if (it->m_metaType == metaType)
				{
          it->Get<ComponentLogic>()->OnDestroy();
					it->m_handle.Destroy();
					m_components.erase(it);
					return;
				}
			}
		}
	}
}