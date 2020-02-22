/*!
  @file Profiling.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Profiling
*/
#pragma once
#include "Core/Macros.hpp"

//TODO: remove this, what a shame to include 2 huge files just for thread id
#include <thread>
#include <functional>

//! @brief Macros for getting global profiling instrumentor
#define GET_GLOBAL_PROFILER() NightEngine::Profiling::Instrumentor::Get()

#if DEBUG_MODE
//! @brief Macros for starting a profiling session
#define PROFILE_SESSION_BEGIN(DESCRIPTION)	\
  GET_GLOBAL_PROFILER().BeginSession(STR_INNER(DESCRIPTION) \
, NightEngine::FileSystem::GetFilePath( STR_CAT(DESCRIPTION, .json) \
  , NightEngine::FileSystem::DirectoryType::Assets))

//! @brief Macros for ending a profiling session
#define PROFILE_SESSION_END()	\
  GET_GLOBAL_PROFILER().EndSession()

//! @brief Macros for profiling a block of code and record it
  #define PROFILE_BLOCK_INSTRUMENT(DESCRIPTION)	\
  for (auto p = std::make_pair(false                                      \
    , NightEngine::Utility::StopWatch{ true });                                  \
	    !(p.first); p.first = true, p.second.Stop()												  \
	  , GET_GLOBAL_PROFILER().WriteProfile(                                 \
      NightEngine::Profiling::ProfileData{DESCRIPTION                            \
    ,p.second.GetStartTimeMicro(), p.second.GetElapsedTimeMicro(), std::hash<std::thread::id>{}(std::this_thread::get_id()) }))
#else
  #define PROFILE_SESSION_BEGIN(DESCRIPTION)
  #define PROFILE_SESSION_END()
  #define PROFILE_BLOCK_INSTRUMENT(DESCRIPTION)
#endif


namespace NightEngine
{
  namespace Profiling
  {
    struct Session
    {
      std::string m_sessionName;
      std::string m_filePath;
    };

    struct ProfileData
    {
      std::string m_name;

      //Both time should be in microsecond unit
      long long m_startTime;  
      float m_elapsedTime;
      
      size_t m_threadId;
    };

    class Instrumentor
    {
    public:
      static Instrumentor& Get();

      void BeginSession(const std::string& sessionName
        , const std::string& filepath = "profile_results.json");
      void EndSession();

      void WriteProfile(const ProfileData& data);

      inline bool IsActive(void) { return m_sessionActive; }
    private:
      void WriteHeader();
      void WriteFooter();

      bool m_sessionActive = false;
      int m_profileCount = 0;
      Session m_session;
    };
  }
} // NightEngine

