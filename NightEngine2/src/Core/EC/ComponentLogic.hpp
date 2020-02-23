/*!
  @file ComponentLogic.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ComponentLogic
*/
#pragma once
#include "Core/Message/IMessageHandler.hpp"
#include "Core/EC/Factory.hpp"
#include "Core/EC/Handle.hpp"

#include "Core/Container/PrimitiveType.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

namespace NightEngine
{
	namespace EC
	{
		//Forward Declaration
		class GameObject;
    class ComponentLogic;

    //! @brief Handle for accessing Component
		struct ComponentHandle
		{
			HandleObject m_handle;	   //Handle for lookup
      Reflection::MetaType* m_metaType;  //Type of the component

      //! @brief Constructor
			ComponentHandle() = default;

      //! @brief Explicit Constructor
			explicit ComponentHandle(GameObject* gameObject
				, HandleObject handle, Reflection::MetaType* metaType);

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
		class ComponentLogic: public NightEngine::IMessageHandler
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
      inline ComponentLogicID GetUID() const{ return m_uniqueID;}

      //! @brief Get Handle to this ComponentLogic
      inline HandleObject GetHandle(void) { return m_handle; }

      //! @brief Get GameObject Reference
      inline const Handle<GameObject>& GetGameObject(void) const { return m_gameObject; }

      //! @brief Set GameObject Reference
      inline void SetGameObject(Handle<GameObject> gameObject) { m_gameObject = gameObject; }

		protected:
			friend struct  ComponentHandle;

			ComponentLogicID      m_uniqueID;	    //Unique id for each component
			Handle<GameObject>    m_gameObject;	  //TODO: Use handle instead
      HandleObject m_handle;       //Handle to itself

			static ComponentLogicID s_uniqueIDCounter;
		};
  }
}

//Template Definition
#include "Core/EC/ComponentLogic.inl"