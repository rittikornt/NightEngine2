/*!
  @file ComponentLogic.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ComponentLogic
*/
#pragma once
#include "Core/Message/IMessageHandler.hpp"
#include "Core/EC/ComponentLogic.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

namespace NightEngine
{
	namespace EC
	{
		//Forward Declaration
		class GameObject;
    class ComponentLogic;

    //**********************************************
		// TEST COMPONENT
    //**********************************************
		class Controller : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
			}

		private:
		};

		class CharacterInfo : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
			}

			void SetMoveSpeed(float speed)
			{
				m_moveSpeed = speed;
			}

			float GetMoveSpeed() const
			{
				return m_moveSpeed;
			}

		private:
			float m_moveSpeed = 10.0f;
			float m_health = 100.0f;
			float m_maxHealth = 100.0f;
		};

		class CTimer : public ComponentLogic
		{
      REFLECTABLE_TYPE();
		public:
			virtual void OnUpdate(float dt) override
			{
				m_timer += dt;
			}

			float GetTimer() const
			{
				return m_timer;
			}

		private:
			float m_timer = 0.0f;
		};
  }
}

//Template Definition
#include "Core/EC/ComponentLogic.inl"