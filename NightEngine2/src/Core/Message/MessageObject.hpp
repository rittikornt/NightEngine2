/*!
  @file MessageObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MessageObject
*/
#pragma once

//! @brief Macros for quick message declaration
#define MSG_GENERATE_METHOD_DECL()	\
virtual void SendMessageTo(IMessageHandler& handler) const override;	

//! @brief Macros for quick message definition in .cpp
#define MSG_GENERATE_METHOD_DEF(TYPE)	\
void TYPE ::SendMessageTo(IMessageHandler & handler) const	\
{	\
	handler.HandleMessage(*this);	\
}

//! @brief Macros for quick message definition in .cpp
#define MSG_GENERATE_METHOD_DEF_HPP(TYPE)	\
virtual void SendMessageTo(IMessageHandler & handler) const	\
{	\
	handler.HandleMessage(*this);	\
}	

namespace Core
{
  //Forward declaration
  class IMessageHandler;
  enum class MessageType : unsigned;

  //! @brief MessageObject base class with virtual method for double dispatch
  struct MessageObject
  {
    //! @brief Default payload data
    union Payload
    {
      void*       m_pointer;
      int         m_int;
      unsigned    m_unsigned;
      long        m_long;
      float       m_float;
      double      m_double;
      bool        m_bool;
      const char* m_chars;
    };

    //! @brief Constructor for setting MessageType enum
    MessageObject(MessageType msgType) : m_msgType(msgType) {}

    //! @brief For sending message directly to handler
    virtual void SendMessageTo(IMessageHandler&) const = 0;

    //**************************************************
    // Public Members
    //**************************************************
    MessageType m_msgType;
    Payload m_payload;
  };
} // Message
