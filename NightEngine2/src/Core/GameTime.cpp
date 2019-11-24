/*!
  @file GameStatus.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameStatus
*/
#include <thread>
#include "Core/Message/MessageObjectList.hpp"
#include "Core/GameTime.hpp"
#include "Core/Logger.hpp"

namespace NightEngine2
{
	using Clock = std::chrono::high_resolution_clock;

  GameTime& GameTime::GetInstance(void)
  {
    static GameTime stat;
    return stat;
  }

  void GameTime::StartFrame()
	{
		// Get the frame start time
		m_frameStartTime = Clock::now();
	}

	void GameTime::EndFrame()
	{
		// Time to sleep before next frame
		auto sleepTime = m_deltaDuration - (Clock::now() - m_frameStartTime);
		if (sleepTime.count() > 0.0f)
		{
			std::this_thread::sleep_for(sleepTime);
		}

		// Time to wait before next frame
		//auto timeTarget = m_frameStartTime + m_deltaDuration;
		//std::this_thread::sleep_until(timeTarget);

		// Calculate the delta time (ms -> second)
		m_deltaTime = FloatDuration(Clock::now() - m_frameStartTime).count();

		// Calculate framerate
		m_frameRate = 1.0f / m_deltaTime;
		m_averageFrameRate = ( m_oldSampleWeight * m_averageFrameRate) 
													+ (m_newSampleWeight * m_frameRate);
	}

	void GameTime::HandleMessage(const Core::GameShouldCloseMessage& msg)
	{
		m_shouldClose = msg.m_shouldClose;
	}
} // namespace World