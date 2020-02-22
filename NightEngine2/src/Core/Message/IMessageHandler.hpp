/*!
  @file IMessageHandler.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of IMessageHandler
*/
#pragma once

#include "MessageObject.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Macros.hpp"

namespace NightEngine
{
	//Forward declaration
  enum class BroadcastScope : uint8_t;
	struct PlayerUpdateMessage;
  struct GameShouldCloseMessage;
  struct TestMessage;
  struct LogMessage;
  struct TransformMessage;

  //! @brief Any class inherit this class will be able to handle message
  class IMessageHandler
  {
		REFLECTABLE_TYPE();

    public:
			virtual ~IMessageHandler();

      void BroadcastMessage(MessageObject& msg, BroadcastScope scope);
      void SendMessage(MessageObject& msg, IMessageHandler& handler);

      void Subscribe(const char* msgName);
			void Subscribe(MessageType msgType);

			void Unsubscribe(MessageType msgType);
			void UnsubscribeAll(void);

      //Unhandled Messages is an error
      virtual void HandleMessage(const MessageObject&) { ASSERT_TRUE(false); }
			
      //Test
      virtual void HandleMessage(const TestMessage&) { ASSERT_TRUE(false); }
      virtual void HandleMessage(const PlayerUpdateMessage&){ ASSERT_TRUE(false); }

      //System
      virtual void HandleMessage(const GameShouldCloseMessage&) { ASSERT_TRUE(false); }
      virtual void HandleMessage(const LogMessage&) { ASSERT_TRUE(false); }
      virtual void HandleMessage(const TransformMessage&) { ASSERT_TRUE(false); }

      //Gameplay

  };
} // NightEngine
