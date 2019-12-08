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
		void StopWatch::Start(void)
		{
			m_startTime = std::chrono::high_resolution_clock::now();
		}

		void StopWatch::Stop(void)
		{
			m_endTime = std::chrono::high_resolution_clock::now();
		}

		float StopWatch::GetElapsedTimeMilli(void)
		{
			return std::chrono::duration<float, std::milli>(m_endTime - m_startTime).count();
		}

    float StopWatch::GetElapsedTimeMicro(void)
    {
      return std::chrono::duration<float, std::micro>(m_endTime - m_startTime).count();
    }

    long long StopWatch::GetStartTimeMilli(void)
    {
      return std::chrono::time_point_cast<std::chrono::milliseconds>(m_startTime).time_since_epoch().count();
    }

    long long StopWatch::GetStartTimeMicro(void)
    {
      return std::chrono::time_point_cast<std::chrono::microseconds>(m_startTime).time_since_epoch().count();
    }

		void Timer::Start(float time)
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
