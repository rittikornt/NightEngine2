/*!
  @file GameStatus.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GameStatus
*/
#pragma once

#include "Core/Message/IMessageHandler.hpp"

#include <chrono>
#include <vector>

namespace NightEngine
{
  //Forward declaration
  struct GameShouldCloseMessage;
}

namespace NightEngine
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using SecondDuration = std::chrono::duration<float>;
	using MilliSeconds = std::chrono::duration<float, std::milli>;

	struct FrameTimeStamp
	{
		int sortIndex = 0;
		const char* name = "";
		float totalTimeMS = 0.0f;
		TimePoint startTime{};
		int indentCount = 0;
	};

	const float c_DEFAULT_FPS_CAP = 60.0f;
	const float c_DEFAULT_FRAME_MS_CAP = 1000.0f/ c_DEFAULT_FPS_CAP;
  class GameTime: public NightEngine::IMessageHandler
  {
  public:
    //! @brief Constructors
		GameTime() = default;
		GameTime(float renderFrameRateCap
      , float simulationFrameRateCap
      , float averageFrameRateSample)
			: m_shouldClose(false)
      //Frame datas
			, m_deltaTimeSeconds(1.0f / renderFrameRateCap)
			, m_currFPS(renderFrameRateCap), m_averageFps(renderFrameRateCap)
			, m_averageFrameRateSampleCount(averageFrameRateSample)
      //Average FPS weights
			, m_oldSampleWeight(((m_averageFrameRateSampleCount - 1.0f) / m_averageFrameRateSampleCount))
			, m_newSampleWeight((1.0f / m_averageFrameRateSampleCount))
			, m_targetFrameTimeSeconds(1.0f / (renderFrameRateCap))
		{
			m_averageFrameTimes = std::vector<float>((size_t)m_averageFrameRateSampleCount, 0.0f);
		}

    //! @brief Get Global Instance
    static GameTime& GetInstance(void);

    //! @brief Start Frame
		void BeginFrame(void);

    //! @brief End Frame
		void EndFrame(void);

		//! @brief Add time stamp for frame time display
		void BeginTimeStamp(const char* name, int sortIndex);

		//! @brief End time stamp for frame time display
		void EndTimeStamp(void);

    //! @brief Handle Close Message
		virtual void HandleMessage(const NightEngine::GameShouldCloseMessage& msg);

  public:
		///////////////////////////////////////////////////////////////////////////
		//	Member Variable
		///////////////////////////////////////////////////////////////////////////
		bool	m_shouldClose = false;
			
    //Frame datas
		float m_deltaTimeSeconds = 1.0f / c_DEFAULT_FPS_CAP;	//Feed into all other system for update
		float m_currFPS = c_DEFAULT_FPS_CAP;									//For display
		float m_averageFps = c_DEFAULT_FPS_CAP;								//For display

    //Time data (seconds)
    float m_timeSinceStartupSeconds = 0.0f;

    //Average FPS weights
		float m_averageFrameRateSampleCount = 15.0f;
		float m_oldSampleWeight = ((m_averageFrameRateSampleCount - 1.0f) / m_averageFrameRateSampleCount);
		float m_newSampleWeight = (1.0f / m_averageFrameRateSampleCount);

    TimePoint m_frameStartTime;
		SecondDuration m_targetFrameTimeSeconds{};

		//Total Frame Time
		MilliSeconds m_currTotalFrameTimeMS{};
		float m_averageFrameTimeMS = 1000.0f / c_DEFAULT_FPS_CAP;
		std::vector<float> m_averageFrameTimes;

		std::vector<FrameTimeStamp> m_frameTimeStampTempStack;
		std::vector<FrameTimeStamp> m_frameTimeStampResult;
  };
} // namespace World
