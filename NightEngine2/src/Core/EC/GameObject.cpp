/*!
  @file GameObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameObject
*/
#include "Core/EC/GameObject.hpp"

#include "Core/Message/MessageObjectList.hpp"
#include "Core/Logger.hpp"

using namespace NightEngine;
using namespace NightEngine::Factory;
using namespace NightEngine::EC::Components;

namespace NightEngine::EC
{
  void GameObject::HandleMessage(const NightEngine::PlayerUpdateMessage& msg)
  {
    Debug::Log << "(" << m_name << ") HandleMessage: " << msg.m_updateAmount << ", Address: " << this << '\n';
  }

  GameObject::GameObject()
    : m_name("Uninitialized")
  {
  }

  GameObject::GameObject(const char* name, size_t reserveSize)
    : m_name(name)
  {
    m_components.reserve(reserveSize);
  }

  GameObject::~GameObject()
  {
  }

  Handle<GameObject> GameObject::Create(const char* name, size_t reserveSize)
  {
    auto handle = Factory::Create<GameObject>("GameObject");
    *handle = GameObject(name, reserveSize);
    handle->m_handle = handle;
    handle->Init();
    return handle;
  }

  void GameObject::Init(void)
  {
    //Initialize the Transform Component
    ASSERT_TRUE(!m_transform.IsValid());

    m_transform = Factory::Create<Transform>("Transform");

    //Init Transform
    auto t = m_transform.Get();
    t->SetGameObject(m_handle);
    t->OnAwake();
  }

  void GameObject::Destroy(void)
  {
    ASSERT_TRUE(m_handle.IsValid());
    RemoveAllComponents();
    m_handle.Destroy();
  }

  Container::Vector<ComponentHandle>& GameObject::GetAllComponents(void)
  {
    return m_components;
  }

  ComponentHandle* GameObject::GetComponent(const char* componentType)
  {
    using namespace Reflection;
    //TODO: Verify if conversion from char* to std::string is valid
    MetaType* metaType = METATYPE_FROM_STRING(componentType);

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

  ComponentHandle* GameObject::AddComponent(const char* componentType)
  {
    //ComponentHandle Constructor will set Component's ref to GameObject
    m_components.emplace_back(this
      , Factory::Create(componentType)
      , METATYPE_FROM_STRING(componentType));

    //Initialize
    auto& handle = m_components.back();
    handle.Get<ComponentLogic>()->SetGameObject(m_handle);
    handle.Get<ComponentLogic>()->OnAwake();

    return &(handle);

    //TODO: Add component to the Space UpdateList
    //Space need to somehow keep track of which index to update
  }

  void GameObject::RemoveComponent(const char* componentType)
  {
    using namespace Reflection;
    //TODO: Verify if conversion from char* to std::string is valid
    MetaType* metaType = METATYPE_FROM_STRING(componentType);

    //Linear Search for Component
    for (auto it = m_components.begin();
      it != m_components.end(); ++it)
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

  void GameObject::RemoveAllComponents()
  {
    for (auto it = m_components.begin()
      ; it != m_components.end(); )
    {
      it->Get<ComponentLogic>()->OnDestroy();
      it->m_handle.Destroy();
      it = m_components.erase(it);
    }
  }
}