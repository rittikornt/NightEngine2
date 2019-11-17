/*!
  @file World.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of World
*/
#include "Core/World.hpp"
#include <iostream>

//++Remove Later, for testing
#include "Core/EC/Factory.hpp"

//++Remove Later, for testing
#include "Core/Message/MessageTypeEnum.hpp"

#include "Core/Reflection/ReflectionCore.hpp"
#include "Core/GameStatus.hpp"
#include "Core/Logger.hpp"

#include "Graphic/GraphicCore.hpp"
#include "Input/Input.hpp"

#include "Core/EC/ArchetypeManager.hpp"

#include "Physics/PhysicsScene.hpp"

using namespace Core;
using namespace Core::Container;
using namespace Core::ECS;

using namespace Physics;

namespace World
{
	//Game Status Setting
	constexpr float      c_FRAMERATE_CAP = 65.0f;
	constexpr float      c_TARGET_DT = 1.0f / c_FRAMERATE_CAP;
	constexpr float      c_AVR_FRAMERATE_SAMPLE = 15.0f;
	static GameStatus*   g_gameStat;
	static PhysicsScene* g_physicScene;

	void Initialize()
	{
		Debug::Log << "World::Initialize\n";

		g_gameStat = &(GameStatus::GetInstance());
		*g_gameStat = GameStatus{ c_FRAMERATE_CAP,c_AVR_FRAMERATE_SAMPLE };
		g_gameStat->Subscribe(Core::MessageType::MSG_GAMESHOULDQUIT);

		//Physics
		g_physicScene = new PhysicsScene();

		//Core
		Reflection::Initialize();
		Factory::Initialize();
		ArchetypeManager::Initialize();

		//Runtime
		Graphic::Initialize();
		Input::Initialize();

		//Physic Init After Graphic
		g_physicScene->Initialize();

		//TODO: Scene Init, Update, Terminate
		//TODO: Spaces Init, Update, Terminate
	}

	void MainLoop(void)
	{
		while (!g_gameStat->m_shouldClose)
		{
			g_gameStat->StartFrame();

			FixedUpdate(g_gameStat->m_deltaTime);
			OnUpdate(g_gameStat->m_deltaTime);

			g_gameStat->EndFrame();
		}
	}

	void FixedUpdate(float dt)
	{
		static float accumulator = 0.0f;
		accumulator += dt;

		while (accumulator > c_TARGET_DT)
		{
			Debug::Log << "World::FixedUpdate(" << c_TARGET_DT
				<< ")\n";

			//++Update here with c_TARGET_DT
			g_physicScene->Update(c_TARGET_DT);

			accumulator -= c_TARGET_DT;
		}

	}

	void OnUpdate(float dt)
	{
		Debug::Log << "World::Update(" << g_gameStat->m_deltaTime
			<< "), FPS: " << g_gameStat->m_frameRate << '\n';

		//Update Systems
		Input::OnUpdate();

		//Render Frame
		Graphic::Render(dt);
	}

	void Terminate()
	{
		Debug::Log << "World::Terminate\n";

		//Terminate System
		Input::Terminate();
		Graphic::Terminate();

		ArchetypeManager::Terminate();
		Factory::Terminate();
		Reflection::Terminate();

		delete g_physicScene;

		g_gameStat->UnsubscribeAll();
	}

} // namespace World
