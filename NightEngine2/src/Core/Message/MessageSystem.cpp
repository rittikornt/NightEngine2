/*!
  @file MessageSystem.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MessageSystem
*/
#include "Core/Message/MessageSystem.hpp"
#include "Core/Message/MessageObject.hpp"
#include "Core/Macros.hpp"

#include <algorithm>


namespace Core
{
#define REGISTER_MESSAGE(MSG) #MSG ,
	const char* msgTypeName[] =
	{
		"MSG_BASE",
#include "Core/Message/RegisterMessageList.inl"
		"MSG_COUNT"
	};
#undef REGISTER_MESSAGE

	const char * MessageSystem::LookupMessageName(MessageType msgType) const
	{
		return msgTypeName[static_cast<size_t>(msgType)];
	}

	void MessageSystem::BroadcastMessage(MessageObject & msg, BroadcastScope scope)
	{
    //TODO: accumulate all message and send at the end of the frame

		const char* msgName = LookupMessageName(msg.m_msgType);
		switch (scope)
		{
			case BroadcastScope::HANDLER:
			{
				//If messageType exist, send msg to all handler
				if (m_msgHandlerMap.find(msgName) != m_msgHandlerMap.end())
				{
					for(auto handler :m_msgHandlerMap[msgName])
					{
						msg.SendMessageTo(*handler);
					}
				}
				break;
			}
			case BroadcastScope::GLOBAL:
			{
				//If messageType exist, send msg to all handler
				if (m_msgHandlerMap.find(msgName) != m_msgHandlerMap.end())
				{
					for (auto handler : m_msgHandlerMap[msgName])
					{
						msg.SendMessageTo(*handler);
					}
				}
				break;
			}
		}
	}

	void MessageSystem::SendMessage(MessageObject& msg, IMessageHandler& handler)
	{
    //TODO: accumulate all message and send at the end of the frame
		msg.SendMessageTo(handler);
	}

	/////////////////////////////////////////////////////////

	void MessageSystem::Subscribe(IMessageHandler& handler, MessageType msgType)
	{
		const char* msgName = LookupMessageName(msgType);
		Subscribe(handler, msgName);
	}

	void MessageSystem::Subscribe(IMessageHandler& handler, const char* msgName)
	{
		//Already registered msgType
		if (m_msgHandlerMap.find(msgName) != m_msgHandlerMap.end())
		{
			//Add to list if not already
			IHandlerList& list = m_msgHandlerMap[msgName];
			//Error if Subscribe to the same msg twice
			ASSERT_TRUE(std::find(list.begin(), list.end(), &handler) == list.end());

			m_msgHandlerMap[msgName].push_back(&handler);
		}
		else
		{
			//Register new msgType
			IHandlerList handlerList{ &handler };
			m_msgHandlerMap[msgName] = handlerList;

			ASSERT_TRUE(m_msgHandlerMap[msgName][0] == &handler);
		}
	}

	/////////////////////////////////////////////////////////

	void MessageSystem::Unsubscribe(IMessageHandler& handler, MessageType msgType)
	{
		const char* msgName = LookupMessageName(msgType);
		Unsubscribe(handler, msgName);
	}

	void MessageSystem::Unsubscribe(IMessageHandler& handler, const char* msgName)
	{
		ASSERT_TRUE(m_msgHandlerMap.find(msgName) != m_msgHandlerMap.end());

		m_msgHandlerMap[msgName].erase(std::remove_if(m_msgHandlerMap[msgName].begin(), m_msgHandlerMap[msgName].end(),
			[&handler](IMessageHandler * i) { return i && (i == &handler); }));
	}

	void MessageSystem::UnsubscribeAll(IMessageHandler& handler)
	{
		//Find handler and remove it from the map
		for (auto& pair : m_msgHandlerMap)
		{
			IHandlerList& list = pair.second;
			auto pos = std::find(list.begin(), list.end(), &handler);
			if (pos != list.end())
			{
				list.erase(pos);
			}
		}
	}

}	// Core
