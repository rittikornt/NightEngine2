/*!
  @file Utility.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Utility
*/
#include "Core/Utility/Utility.hpp"

namespace Core
{
	namespace Utility
	{
		void StopWatch::OnStart(void)
		{
			m_startTime = std::chrono::high_resolution_clock::now();
		}

		void StopWatch::Stop(void)
		{
			m_endTime = std::chrono::high_resolution_clock::now();
		}

		float StopWatch::GetElapsedTime(void)
		{
			return std::chrono::duration<float, std::milli>(m_endTime - m_startTime).count();
		}

		void Timer::OnStart(float time)
		{
			m_curTime = time;
			m_done = false;
		}

		void Timer::Stop(void)
		{
			m_done = true;
		}

		bool Timer::OnUpdate(float dt)
		{
			if (m_done)
			{
				return true;
			}
			
			m_curTime -= dt;
			return m_curTime < 0.0f ? true : false;
		}
	} // Utility
} // Core
