/*!
  @file Engine.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Engine
*/
#include "NightEngine2.hpp"
#include <iostream>

//++Remove Later, for testing
#include "Core/EC/Factory.hpp"

//++Remove Later, for testing
#include "Core/Message/MessageTypeEnum.hpp"

#include "Core/Reflection/ReflectionCore.hpp"
#include "Core/Logger.hpp"

#include "Core/GameTime.hpp"
#include "Input/Input.hpp"

#include "Core/EC/ArchetypeManager.hpp"

#include "Physics/PhysicsScene.hpp"
#include "Graphic/Opengl/RenderLoopOpengl.hpp"

using namespace Core;
using namespace Core::Container;
using namespace Core::ECS;

using namespace Graphic;
using namespace Physics;

namespace NightEngine2
{
  //Game Status Setting
  constexpr float      c_FRAMERATE_CAP = 65.0f;
  constexpr float      c_TARGET_DT = 1.0f / c_FRAMERATE_CAP;
  constexpr float      c_AVR_FRAMERATE_SAMPLE = 15.0f;

  //TODO: there should be one per scene
  static PhysicsScene* g_physicScene = nullptr;

  void Engine::Initialize(void)
  {
    Debug::Log << "Engine::Initialize\n";

    m_gameTime = &(GameTime::GetInstance());
    *m_gameTime = GameTime{ c_FRAMERATE_CAP,c_AVR_FRAMERATE_SAMPLE };
    m_gameTime->Subscribe(Core::MessageType::MSG_GAMESHOULDQUIT);

    //Physics
    g_physicScene = new PhysicsScene();

    //Core
    Reflection::Initialize();
    Factory::Initialize();
    ArchetypeManager::Initialize();

    //Runtime
    m_renderloop = new RenderLoopOpengl();
    m_renderloop->Initialize();
    Input::Initialize();

    //Physic Init After Graphic
    g_physicScene->Initialize();

    //TODO: Scene Init, Update, Terminate
    //TODO: Spaces Init, Update, Terminate
  }

  void Engine::MainLoop(void)
  {
    while (!m_gameTime->m_shouldClose)
    {
      m_gameTime->StartFrame();

      FixedUpdate(m_gameTime->m_deltaTime);
      OnUpdate(m_gameTime->m_deltaTime);

      m_gameTime->EndFrame();
    }
  }

  void Engine::FixedUpdate(float dt)
  {
    static float accumulator = 0.0f;
    accumulator += dt;

    while (accumulator > c_TARGET_DT)
    {
      //Debug::Log << "Engine::FixedUpdate(" << c_TARGET_DT << ")\n";

      //++Update here with c_TARGET_DT
      g_physicScene->Update(c_TARGET_DT);

      accumulator -= c_TARGET_DT;
    }

  }

  void Engine::OnUpdate(float dt)
  {
    //Debug::Log << "Engine::Update(" << g_gameStat->m_deltaTime
    //  << "), FPS: " << g_gameStat->m_frameRate << '\n';

    //Update Systems
    Input::OnUpdate();

    //TODO: Update all the Components

    //Render Frame
    m_renderloop->Render(dt);
  }

  void Engine::Terminate(void)
  {
    Debug::Log << "Engine::Terminate\n";

    //Terminate System
    Input::Terminate();
    m_renderloop->Terminate();

    ArchetypeManager::Terminate();
    Factory::Terminate();
    Reflection::Terminate();

    delete m_renderloop;
    delete g_physicScene;

    m_gameTime->UnsubscribeAll();
  }

} // namespace World
