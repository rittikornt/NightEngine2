/*!
  @file TestComponent.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Test Components
*/
#pragma once
#include "Core/Message/IMessageHandler.hpp"
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/EC/Components/Transform.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include <glm/vec3.hpp>

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
			REFLECTABLE_TYPE_BLOCK()
			{
				META_REGISTERER_WITHBASE(Controller, ComponentLogic
					, InheritType::PUBLIC, true
					, nullptr, nullptr)
					.MR_ADD_MEMBER_PROTECTED(Controller, m_rotateSpeed, true)
					.MR_ADD_MEMBER_PROTECTED(Controller, m_upAngle, true);
			}
		public:
			virtual void OnUpdate(float dt) override
			{
				m_upAngle += (dt * m_rotateSpeed);
				const float k_degToRadian = 0.0174533f;
				float upRadian = m_upAngle* k_degToRadian * m_rotateSpeed;

				auto t = this->GetGameObject()->GetTransform();
				t->SetEulerAngle(glm::vec3(0.0f, upRadian, 0.0f));
			}

		private:
			float m_rotateSpeed = -20.0f;
			float m_upAngle = 0.0f;
		};

		class CharacterInfo : public ComponentLogic
		{
			REFLECTABLE_TYPE_BLOCK()
			{
				META_REGISTERER_WITHBASE(CharacterInfo, ComponentLogic
					, InheritType::PUBLIC, true
					, nullptr, nullptr)
					.MR_ADD_MEMBER_PROTECTED(CharacterInfo, m_moveSpeed, true)
					.MR_ADD_MEMBER_PROTECTED(CharacterInfo, m_health, false)
					.MR_ADD_MEMBER_PROTECTED(CharacterInfo, m_maxHealth, true);
			}
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
			REFLECTABLE_TYPE_BLOCK()
			{
				META_REGISTERER_WITHBASE(CTimer, ComponentLogic
					, InheritType::PUBLIC, true
					, nullptr, nullptr)
					.MR_ADD_MEMBER_PROTECTED(CTimer, m_timer, true);
			}
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