/*!
  @file GameStatus.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GameStatus
*/
#pragma once

#include <chrono>
#include "Core/Message/IMessageHandler.hpp"

namespace NightEngine
{
  //Forward declaration
  struct GameShouldCloseMessage;
}

namespace NightEngine
{
	constexpr float c_DEFAULT_FPS_CAP = 60.0f;
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
			, m_deltaTime(1.0f / renderFrameRateCap)
			, m_fps(renderFrameRateCap), m_averageFps(renderFrameRateCap)
			, m_averageFrameRateSample(averageFrameRateSample)
      //Average FPS weights
			, m_oldSampleWeight(((m_averageFrameRateSample - 1.0f) / m_averageFrameRateSample))
			, m_newSampleWeight((1.0f / m_averageFrameRateSample))
			, m_deltaDuration(1.0f / (renderFrameRateCap))
		{}

    //! @brief Get Global Instance
    static GameTime& GetInstance(void);

    //! @brief Start Frame
		void StartFrame();

    //! @brief End Frame
		void EndFrame();

    //! @brief Handle Close Message
		virtual void HandleMessage(const NightEngine::GameShouldCloseMessage& msg);

  public:
		///////////////////////////////////////////////////////////////////////////
		//	Member Variable
		///////////////////////////////////////////////////////////////////////////
		using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using FloatDuration = std::chrono::duration<float>;

		bool	m_shouldClose = false;
			
    //Frame datas
		float m_deltaTime = 1.0f / c_DEFAULT_FPS_CAP;				//Feed into all other system for update
		float m_fps = c_DEFAULT_FPS_CAP;										//For display
		float m_averageFps = c_DEFAULT_FPS_CAP;							//For display

    //Time data (seconds)
    float m_timeSinceStartup = 0.0f;

    //Average FPS weights
		float m_averageFrameRateSample = 15;
		float m_oldSampleWeight = ((m_averageFrameRateSample - 1.0f) / m_averageFrameRateSample);
		float m_newSampleWeight = (1.0f / m_averageFrameRateSample);

    TimePoint m_frameStartTime;
		FloatDuration m_deltaDuration{};
  };
} // namespace World
