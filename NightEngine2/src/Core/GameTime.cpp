/*!
  @file GameStatus.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameStatus
*/
#include <thread>
#include "Core/Message/MessageObjectList.hpp"
#include "Core/GameTime.hpp"
#include "Core/Logger.hpp"

namespace NightEngine
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
    //if (sleepTime.count() > 0.0f)
    //{
    //	std::this_thread::sleep_for(sleepTime);
    //}

    //Do spinlock-like wait
    auto sleepTime = m_deltaDuration - (Clock::now() - m_frameStartTime);
    while (sleepTime.count() >= 0.0f)
    {
      sleepTime = m_deltaDuration - (Clock::now() - m_frameStartTime);
    }

		// Calculate the delta time (ms -> second)
		m_deltaTime = FloatDuration(Clock::now() - m_frameStartTime).count();

    //Update Time
    m_timeSinceStartup += m_deltaTime;

		// Calculate framerate
		m_fps = 1.0f / m_deltaTime;
		m_averageFps = ( m_oldSampleWeight * m_averageFps) 
													+ (m_newSampleWeight * m_fps);
	}

	void GameTime::HandleMessage(const NightEngine::GameShouldCloseMessage& msg)
	{
		m_shouldClose = msg.m_shouldClose;
	}
} // namespace World