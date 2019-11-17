/*!
  @file Logger.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Logger
*/
#include "Core/Logger.hpp"
#include "Core/Macros.hpp"

#include "Core/Message/MessageSystem.hpp"
#include "Core/Message/MessageObjectList.hpp"

namespace Core
{
	namespace Debug
	{
    //Global Logger
		Logger Log;
	}

	Logger::Logger(): m_curMessageType(MessageType::INFO) {}

	Logger::~Logger()
	{
    if (m_outfileStream.is_open())
    {
      m_outfileStream.close();
    }
	}

	void Logger::SetFileOutputStream(const std::string& filename)
	{
		// Open log file
		std::string path = PROJECT_DIR_LOGFILE;
		path += filename;
		m_outfileStream.open(path);

		//Check error
		if (!m_outfileStream.is_open())
		{
			*this << MessageType::ERROR_MSG << "Could not open log file " << path.c_str()
				<< "!" << std::endl;
		}

		// Print init text
		*this << MessageType::INFO << "Logger initialized: " << filename <<
			std::endl;
	}

	void Logger::SendLogToDevConsole()
	{
    std::string log{m_logStream.str()};
    LogMessage logMsg{ log };
    MessageSystem::Get().BroadcastMessage(logMsg
    , BroadcastScope::HANDLER);

    //Clear stringstream
    m_logStream.str(std::string());
    m_logStream.clear();
	}

  Logger & Logger::operator<<(const std::string& message)
  {
    using namespace Core;

    // Send message to Logger
    std::cout << message;

    // Send to DevConsole
    m_logStream << message;
    if (message.find_first_of('\n') != std::string::npos)
    {
      SendLogToDevConsole();
    }

    if (m_outfileStream.is_open())
    {
      m_outfileStream << message;
    }

    return *this;
  }

  Logger & Logger::operator<<(const char* message)
  {
    using namespace Core;

    // Send message to Logger
    std::cout <<  message;

    // Send to DevConsole
    m_logStream << message;
    if (strstr(message, "\n"))
    {
      SendLogToDevConsole();
    }

    if (m_outfileStream.is_open())
    {
      m_outfileStream << message;
    }

    return *this;
  }

  Logger & Logger::operator<<(const char & message)
  {
    using namespace Core;

    // Send message to Logger
    std::cout << message;

    // Send to DevConsole
    m_logStream << message;
    if (message == '\n')
    {
      SendLogToDevConsole();
    }

    if (m_outfileStream.is_open())
    {
      m_outfileStream << message;
    }

    return *this;
  }

  Logger & Logger::operator<<(MessageType type)
	{
		m_curMessageType = type;
		
		// TODO: Newline to DevConsole
		switch (type)
		{
			case MessageType::ERROR_MSG:
			{
				std::cout << "[error] ";
        m_logStream << "[error] ";
				break;
			}
			case MessageType::INFO:
			{
				std::cout << "[info] ";
        m_logStream << "[info] ";
				break;
			}
			case MessageType::WARNING:
			{
				std::cout << "[warning] ";
        m_logStream << "[warning] ";
				break;
			}
      case MessageType::LUA:
      {
        std::cout << "[lua] ";
        m_logStream << "[lua] ";
        break;
      }
		}

		return *this;
	}

	Logger & Logger::operator<<(std::ostream &(*OutStreamFn)(std::ostream &))
	{
    OutStreamFn(std::cout);
		if (m_outfileStream.is_open())
		{
      OutStreamFn(m_outfileStream);
		}

		return *this;
	}
}
