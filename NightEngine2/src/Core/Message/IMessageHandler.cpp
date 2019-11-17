/*!
  @file IMessageHandler.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of IMessageHandler
*/
#include "Core/Message/IMessageHandler.hpp"
#include "Core/Message/MessageSystem.hpp"

namespace Core
{
  IMessageHandler::~IMessageHandler()
  {
  }

  void IMessageHandler::BroadcastMessage(MessageObject & msg, BroadcastScope scope)
  {
    MessageSystem::Get().BroadcastMessage(msg, scope);
  }

  void IMessageHandler::SendMessage(MessageObject & msg, IMessageHandler & handler)
  {
    MessageSystem::Get().SendMessage(msg, handler);
  }

  void IMessageHandler::Subscribe(const char* msgName)
  {
    MessageSystem::Get().Subscribe(*this, msgName);
  }

  void IMessageHandler::Subscribe(MessageType msgType)
  {
    MessageSystem::Get().Subscribe(*this, msgType);
  }

  void IMessageHandler::Unsubscribe(MessageType msgType)
  {
    MessageSystem::Get().Unsubscribe(*this, msgType);
  }

  void IMessageHandler::UnsubscribeAll(void)
  {
    MessageSystem::Get().UnsubscribeAll(*this);
  }
}

//////////////////////////////////////////////////////////
