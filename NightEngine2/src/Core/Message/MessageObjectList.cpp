/*!
  @file MessageObjectList.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MessageObjectList
*/
#include "Core/Message/MessageObjectList.hpp"
#include "Core/Message/IMessageHandler.hpp"   //Definition

//Header of the Message Declaration

namespace NightEngine
{
  //All MessageObject should be Define here
	MSG_GENERATE_METHOD_DEF(TestMessage)

	MSG_GENERATE_METHOD_DEF(PlayerUpdateMessage)

	MSG_GENERATE_METHOD_DEF(GameShouldCloseMessage)

  MSG_GENERATE_METHOD_DEF(LogMessage)

  MSG_GENERATE_METHOD_DEF(TransformMessage)
    
	////////////////////////////////////////////////

}
