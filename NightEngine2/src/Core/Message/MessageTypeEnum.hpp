/*!
  @file MessageTypeEnum.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MessageTypeEnum
*/
#pragma once

namespace NightEngine
{
  //! @brief Should only be include in the .cpp file
#define REGISTER_MESSAGE(MSG) MSG,
	enum class MessageType : unsigned
	{
		MSG_BASE = 0,
#include "RegisterMessageList.inl"
		MSG_COUNT
	};
#undef REGISTER_MESSAGE

} // NightEngine
