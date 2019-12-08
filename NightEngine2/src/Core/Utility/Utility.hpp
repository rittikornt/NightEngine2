/*!
  @file Utility.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Utility
*/
#pragma once
#include <chrono>
#include "Core/Logger.hpp"

//! @brief Macros for quick profiling a block of code
#define PROFILE_BLOCK(DESCRIPTION)	\
Core::Debug::Log << "\n//*************************************************\n" \
								 << "// Start PROFILE_BLOCK(" << DESCRIPTION << ")\n" \
								 << "//*************************************************\n\n"; \
for (auto p = std::make_pair(false, Core::Utility::StopWatch{ true })	\
	; !(p.first); p.first = true, p.second.Stop()												\
	, Core::Debug::Log << "\n//*************************************************\n" \
	<< "// End PROFILE_BLOCK(" << DESCRIPTION << "): "									\
	<< p.second.GetElapsedTimeMilli() << " ms\n" 														\
  << "//*************************************************\n\n")

//! @brief Macros for quick profiling a block of code
#define PROFILE_BLOCK_SINGLELINE(DESCRIPTION)	\
for (auto p = std::make_pair(false, Core::Utility::StopWatch{ true })	\
	; !(p.first); p.first = true, p.second.Stop()												\
	, Core::Debug::Log << "\n//*************************************************\n" \
	<< "// End PROFILE_BLOCK(" << DESCRIPTION << "): "									\
	<< p.second.GetElapsedTimeMilli() << " ms\n" 														\
  << "//*************************************************\n")

namespace Core
{
  namespace Utility
  {
		//! @brief Class for measuring time, use for simple profiling
		class StopWatch
		{
		public:
			explicit StopWatch(bool start) 
			{
				if (start) Start();
			}

			void Start(void);
			void Stop(void);

			float GetElapsedTimeMilli(void);
      float GetElapsedTimeMicro(void);

      long long GetStartTimeMilli(void);
      long long GetStartTimeMicro(void);
		private:
			using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
			TimePoint m_startTime;
			TimePoint m_endTime;
		};

		//! @brief Class for counting down time
		class Timer
		{
		public:
			explicit Timer(float time, bool start)
				: m_curTime(time), m_done(!start) {}

			void Start(float time);
			void Stop(void);
			bool OnUpdate(float dt);	//Return true if timer done
		private:
			float m_curTime;
			bool m_done;
		};
  } // Utility
} // Core

