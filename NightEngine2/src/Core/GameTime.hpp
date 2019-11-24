/*!
  @file GameStatus.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GameStatus
*/
#pragma once

#include <chrono>
#include "Core/Message/IMessageHandler.hpp"

namespace Core
{
  //Forward declaration
  struct GameShouldCloseMessage;
}

namespace NightEngine2
{
	constexpr float c_DEFAULT_FPS_CAP = 60.0f;
  struct GameTime: public Core::IMessageHandler
  {
    //! @brief Constructors
		GameTime() = default;
		GameTime(float frameRateCap, float averageFrameRateSample)
			: m_shouldClose(false), m_frameRateCap(frameRateCap)
			, m_deltaTime(1.0f / frameRateCap)
			, m_frameRate(frameRateCap), m_averageFrameRate(frameRateCap)
			, m_averageFrameRateSample(averageFrameRateSample)
			, m_oldSampleWeight(((m_averageFrameRateSample - 1.0f) / m_averageFrameRateSample))
			, m_newSampleWeight((1.0f / m_averageFrameRateSample))
			, m_deltaDuration(1.0f / (m_frameRateCap))
		{}

    //! @brief Get Global Instance
    static GameTime& GetInstance(void);

    //! @brief Start Frame
		void StartFrame();

    //! @brief End Frame
		void EndFrame();

    //! @brief Handle Close Message
		virtual void HandleMessage(const Core::GameShouldCloseMessage& msg);

		///////////////////////////////////////////////////////////////////////////
		//	Public Variable
		///////////////////////////////////////////////////////////////////////////
		using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using FloatDuration = std::chrono::duration<float>;

		bool	m_shouldClose = false;
		float m_frameRateCap = c_DEFAULT_FPS_CAP;
			
		float m_deltaTime = 1.0f / c_DEFAULT_FPS_CAP;							//Feed into all other system for update
		float m_frameRate = c_DEFAULT_FPS_CAP;										//For display
		float m_averageFrameRate = c_DEFAULT_FPS_CAP;							//For display

		float m_averageFrameRateSample = 15;
		float m_oldSampleWeight = ((m_averageFrameRateSample - 1.0f) / m_averageFrameRateSample);
		float m_newSampleWeight = (1.0f / m_averageFrameRateSample);

    TimePoint m_frameStartTime;
		FloatDuration m_deltaDuration;
  };
} // namespace World
