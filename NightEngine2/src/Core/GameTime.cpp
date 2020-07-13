/*!
  @file GameStatus.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameStatus
*/
#include "Core/Message/MessageObjectList.hpp"
#include "Core/GameTime.hpp"
#include "Core/Logger.hpp"

#include <thread>

namespace NightEngine
{
	using Clock = std::chrono::high_resolution_clock;

  GameTime& GameTime::GetInstance(void)
  {
    static GameTime stat;
    return stat;
  }

  void GameTime::BeginFrame()
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

    //Total Frame Time
    auto currTotalFrameTimeNS = (Clock::now() - m_frameStartTime);
    m_currTotalFrameTimeMS = std::chrono::duration_cast<MilliSeconds>(currTotalFrameTimeNS);

    //Do spinlock-like wait
    auto sleepTime = m_targetFrameTimeSeconds - currTotalFrameTimeNS;
    while (sleepTime.count() >= 0.0f)
    {
      sleepTime = m_targetFrameTimeSeconds - (Clock::now() - m_frameStartTime);
    }

		// Calculate the delta time (ms -> second)
    m_deltaTimeSeconds = SecondDuration(Clock::now() - m_frameStartTime).count();
    m_timeSinceStartupSeconds += m_deltaTimeSeconds;

		// Calculate framerate
		m_currFPS = 1.0f / m_deltaTimeSeconds;
		m_averageFps = ( m_oldSampleWeight * m_averageFps) 
													+ (m_newSampleWeight * m_currFPS);

    //Record new avr total frame time
    float currTotalFrameTimeMS = m_currTotalFrameTimeMS.count();
    m_averageFrameTimeMS = (m_oldSampleWeight * m_averageFrameTimeMS)
      + (m_newSampleWeight * (currTotalFrameTimeMS));

    //Fill in the data from the back
    size_t size = m_averageFrameTimes.size();
    for (size_t i = 0; i < size -1; ++i)
    {
      m_averageFrameTimes[i] = m_averageFrameTimes[i + 1u];
    }
    m_averageFrameTimes[size - 1] = currTotalFrameTimeMS;

    //Clear time stamp
    m_frameTimeStampTempStack.clear();
    m_frameTimeStampResult.clear();
	}

  void GameTime::BeginTimeStamp(const char* name, int sortIndex)
  {
    FrameTimeStamp fts;
    fts.sortIndex = sortIndex;
    fts.name = name;
    fts.startTime = Clock::now();
    fts.totalTimeMS = 0.0f;
    fts.indentCount = m_frameTimeStampTempStack.size();
    m_frameTimeStampTempStack.push_back(fts);
  }

  void GameTime::EndTimeStamp()
  {
    if (m_frameTimeStampTempStack.size() > 0)
    {
      //Calculate total time ms passed since startTime
      auto& frameTime = m_frameTimeStampTempStack[m_frameTimeStampTempStack.size() - 1];
      auto timeNS = (Clock::now() - frameTime.startTime);
      float timeMS = std::chrono::duration_cast<MilliSeconds>(timeNS).count();
      frameTime.totalTimeMS = timeMS;

      m_frameTimeStampResult.push_back(frameTime);
      m_frameTimeStampTempStack.pop_back();

      //End of the first time stamp, sort the result in ascending order
      if (frameTime.sortIndex == 0)
      {
        std::sort(m_frameTimeStampResult.begin(), m_frameTimeStampResult.end(),
          [](const FrameTimeStamp& a, const FrameTimeStamp& b) -> bool
          {
            return a.sortIndex < b.sortIndex;
          });
      }
    }
  }

	void GameTime::HandleMessage(const NightEngine::GameShouldCloseMessage& msg)
	{
		m_shouldClose = msg.m_shouldClose;
	}
} // namespace World