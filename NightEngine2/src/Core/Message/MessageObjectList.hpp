/*!
  @file MessageObjectList.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MessageObjectList
*/
#pragma once
#include <sstream>
#include <glm/vec3.hpp>

#include "Core/Message/MessageObject.hpp"
#include "Core/Message/MessageTypeEnum.hpp"		//Enum List


namespace Core
{
	struct TestMessage : public MessageObject
	{
		//Constructor
    TestMessage(bool toggle, int incrementAmount) : 
      MessageObject(MessageType::MSG_TEST), m_toggle(toggle)
      , m_incrementAmount(incrementAmount)
    {}

		//Override Double dispatch
    MSG_GENERATE_METHOD_DECL()

		bool m_toggle = false;
		int m_incrementAmount = 1;
	};

  struct PlayerUpdateMessage : public MessageObject
	{
		enum class UpdateType : unsigned char
		{
			HEALTHCHANGED,
			MPCHANGED
		};

		//Constructor
    PlayerUpdateMessage(UpdateType type, unsigned amount)
      :MessageObject(MessageType::MSG_PLAYERUPDATE), m_updateType(type), m_updateAmount(amount)
    {}

		//Override Double dispatch
		MSG_GENERATE_METHOD_DECL()

		UpdateType m_updateType;
		unsigned m_updateAmount;
	};

  struct GameShouldCloseMessage : public MessageObject
	{
		//Constructor
    GameShouldCloseMessage(bool shouldClose) :
      MessageObject(MessageType::MSG_GAMESHOULDQUIT), m_shouldClose(shouldClose)
    {}

		//Override Double dispatch
		MSG_GENERATE_METHOD_DECL()

		bool m_shouldClose;
	};

  //! @brief For sending message to DevConsole
  struct LogMessage : public MessageObject
  {
    //Constructor
    LogMessage(std::string& str)
      :MessageObject(MessageType::MSG_LOGMESSAGE), m_string(str)
    {}

    //Override Double dispatch
    MSG_GENERATE_METHOD_DECL()

    std::string m_string;
  };

  //! @brief For sending message to DevConsole
  struct TransformMessage : public MessageObject
  {
    enum class TransformType : unsigned char
    {
      TRANSLATE,
      ROTATE,
      SCALE
    };

    //Constructor
    TransformMessage(const std::string& targetName ,TransformType type, glm::vec3 amount)
      :MessageObject(MessageType::MSG_TRANSFORMMESSAGE)
      , m_targetName(targetName),m_type(type), m_amount(amount)
    {}

    //Override Double dispatch
    MSG_GENERATE_METHOD_DECL()

    std::string   m_targetName;
    TransformType m_type;
    glm::vec3     m_amount;
  };
} // Message

