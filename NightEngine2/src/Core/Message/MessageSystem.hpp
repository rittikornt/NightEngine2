/*!
  @file MessageSystem.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MessageSystem
*/
#pragma once

#include "Core/Container/Hashmap.hpp"
#include "Core/Container/Vector.hpp"
#include "Core/Container/MurmurHash2.hpp"
#include <string>

namespace Core
{
	//Forward declaration
	class IMessageHandler;
	struct MessageObject;
	enum class MessageType : unsigned;

  enum class BroadcastScope : uint8_t
  {
    HANDLER = 0,
    GLOBAL
  };

  //! @brief MessageSystem Singleton for sending message
	class MessageSystem
	{
	public:
		static MessageSystem& Get()
		{
			//Threadsafe Singleton in C++11
			static MessageSystem instance;
			return instance;
		}

		const char* LookupMessageName(MessageType) const;
		void BroadcastMessage(MessageObject&, BroadcastScope);

		void SendMessage(MessageObject &, IMessageHandler &);

		void Subscribe(IMessageHandler &, MessageType);
		void Subscribe(IMessageHandler &, const char*);

		void Unsubscribe(IMessageHandler &, MessageType);
		void Unsubscribe(IMessageHandler &, const char*);

		void UnsubscribeAll(IMessageHandler &);
	private:
		struct StringHash 
		{
				std::size_t operator()(const std::string& key) const 
				{
					return Core::Container::ConvertToHash(key.c_str(), key.size());
				}
		};
		using IHandlerList = Container::Vector<IMessageHandler *>;
		using MessageMap = Container::HashmapCustom< std::string, IHandlerList, StringHash>;

		MessageMap m_msgHandlerMap;
	};

} // namespace Core
