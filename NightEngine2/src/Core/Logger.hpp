/*!
  @file Logger.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Logger
*/
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#define DEBUG_INFO NightEngine::Debug::Log << NightEngine::Logger::MessageType::INFO
#define DEBUG_WARNING NightEngine::Debug::Log << NightEngine::Logger::MessageType::WARNING
#define DEBUG_ERROR NightEngine::Debug::Log << NightEngine::Logger::MessageType::ERROR_MSG

namespace NightEngine
{
	class Logger
	{
	public:

    //! @brief Enum describing MessageType
		enum class MessageType : unsigned
		{
			INFO = 0,
      ERROR_MSG,
			WARNING,
      LUA
		};

		///////////////////////////////////////////////////////

		Logger();
		~Logger();

    //! @brief Set target output stream
		void SetFileOutputStream(const std::string& filename);

    //! @brief Broadcast LogMessage to DevConsole
		void SendLogToDevConsole();

		//! @brief For sending things to Logger
		template<typename ... T>
		void Print(MessageType mode, const T& ... t)
		{
			*this << mode;
			std::initializer_list<int>{ (*this << t, 0)... };
		}

		//! @brief For sending message to Logger
		template<typename T>
		Logger& operator<<(const T& message)
		{
      using namespace NightEngine;

			// Send message to Logger
			std::cout << message;

      // Stream to send to DevConsole
      m_logStream << message;

			if (m_outfileStream.is_open())
			{
				m_outfileStream << message;
			}

			return *this;
		}

    //! @brief For sending message to Logger
    Logger& operator<<(const std::string& message);

    //! @brief For sending message to Logger
    Logger& operator<<(const char* message);

    //! @brief For sending message to Logger
    Logger& operator<<(const char& message);

    //! @brief For sending message to Logger
		Logger& operator<<(MessageType type);

    //! @brief For sending message to Logger
		Logger& operator<<(std::ostream& (*OutStreamFn)(std::ostream&));

	private:
		std::ofstream m_outfileStream;	//For writing Log to text file
    std::stringstream m_logStream;  //For accumulating before sending it to DevConsole
    MessageType m_curMessageType;	
	};

	namespace Debug
	{
		//! @brief Global Logger
		extern Logger Log;
	}

}

