/*!
  @file Profiling.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Profiling
*/
#include "Core/Utility/Profiling.hpp"
#include "Core/Logger.hpp"

namespace NightEngine
{
	namespace Profiling
	{
    static std::ofstream g_outputStream;

    Instrumentor& Instrumentor::Get()
    {
      static Instrumentor instance;
      return instance;
    }

    void Instrumentor::BeginSession(const std::string& sessionName
      , const std::string& filepath)
    {
      m_session = Session{ sessionName, filepath };

      g_outputStream.open(filepath);
      WriteHeader();

      m_sessionActive = true;
      Debug::Log << "BeginProfilingSession: " << sessionName << "\n";
    }

    void Instrumentor::EndSession()
    {
      Debug::Log << "EndProfilingSession: " << m_session.m_sessionName << "\n";

      WriteFooter();
      g_outputStream.close();
      m_profileCount = 0;

      m_sessionActive = false;
    }

    void Instrumentor::WriteProfile(const ProfileData& data)
    {
      if (m_sessionActive)
      {
        if (m_profileCount++ > 0)
        {
          g_outputStream << ",";
        }

        g_outputStream << "{";
        g_outputStream << "\"cat\":\"function\",";
        g_outputStream << "\"dur\":" << (data.m_elapsedTime) << ',';
        g_outputStream << "\"name\":\"" << data.m_name << "\",";
        g_outputStream << "\"ph\":\"X\",";
        g_outputStream << "\"pid\":0,";
        g_outputStream << "\"tid\":" << data.m_threadId << ",";
        g_outputStream << "\"ts\":" << data.m_startTime;
        g_outputStream << "}";

        g_outputStream.flush();
      }
    }

    void Instrumentor::WriteHeader()
    {
      g_outputStream << "{\"otherData\": {},\"traceEvents\":[";
      g_outputStream.flush();
    }

    void Instrumentor::WriteFooter()
    {
      g_outputStream << "]}";
      g_outputStream.flush();
    }

	} // Profiling
} // NightEngine
