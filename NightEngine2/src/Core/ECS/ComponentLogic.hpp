/*!
  @file ComponentLogic.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ComponentLogic
*/
#pragma once
#include "Core/Message/IMessageHandler.hpp"
#include "Core/ECS/Factory.hpp"

#include "Core/Container/PrimitiveType.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

namespace Core
{
	namespace ECS
	{
		//Forward Declaration
		class GameObject;
    class ComponentLogic;

    //! @brief Handle for accessing Component
		struct ComponentHandle
		{
			Factory::HandleObject m_handle;	   //Handle for lookup
      Reflection::MetaType* m_metaType;  //Type of the component

      //! @brief Constructor
			ComponentHandle() = default;

      //! @brief Explicit Constructor
			explicit ComponentHandle(GameObject* gameObject
				, Factory::HandleObject handle, Reflection::MetaType* metaType);

      //! @brief Getter
      template<typename T>
      T* Get(void)
			{
				return m_handle.Get<T>();
			}

      //! @brief Getter
      void* GetPointer(void)
      {
        return m_handle.GetPointer();
      }

      //! @brief Getter
			bool operator==(const ComponentHandle& rhs) const
			{
				return m_handle == rhs.m_handle;
			}
		};

    using ComponentLogicID = Container::U64;
		//TODO: someway of storing all type inheriting from ComponentLogic
		// for iterating each Component Container in Scene Update()
		// Possible Solution: Template Curiously Recurring Pattern (AutoLister<T>)
		class ComponentLogic: public Core::IMessageHandler
		{
			REFLECTABLE_TYPE();
    public:
      //! @brief Constructor
      ComponentLogic() : m_uniqueID(s_uniqueIDCounter++) {}

      //! @brief Destructor
      virtual ~ComponentLogic() {}

      //! @brief Awake callback
      virtual void OnAwake(void) {}

      //! @brief Start callback
      virtual void OnStart(void) {}

      //! @brief Update callback
      virtual void OnUpdate(float dt) {}

      //! @brief Destroy callback
      virtual void OnDestroy(void) {}

      //! @brief Get Unique ID of this component
      ComponentLogicID GetUID() const{ return m_uniqueID;}

      //! @brief Get Handle to this ComponentLogic
      Factory::HandleObject GetHandle(void) { return m_handle; }

      //! @brief Get GameObject Reference
      inline GameObject* GetGameObject(void) const { return m_gameObject; }

      //! @brief Set GameObject Reference
      void SetGameObject(GameObject* gameObject) { m_gameObject = gameObject; }

		protected:
			friend struct  ComponentHandle;

			ComponentLogicID      m_uniqueID;	    //Unique id for each component
			GameObject*           m_gameObject;	  //TODO: Use handle instead
      Factory::HandleObject m_handle;       //Handle to itself

			static ComponentLogicID s_uniqueIDCounter;
		};

    //**********************************************
		// TEST COMPONENT
    //**********************************************
		class Controller : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
			}

		private:
		};

		class CharacterInfo : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
			}

			void SetMoveSpeed(float speed)
			{
				m_moveSpeed = speed;
			}

			float GetMoveSpeed() const
			{
				return m_moveSpeed;
			}

		private:
			float m_moveSpeed = 10.0f;
			float m_health = 100.0f;
			float m_maxHealth = 100.0f;
		};

		class CTimer : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
				m_timer += dt;
			}

			float GetTimer() const
			{
				return m_timer;
			}

		private:
			float m_timer = 0.0f;
		};
  }
}

//Template Definition
#include "Core/ECS/ComponentLogic.inl"