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
#include "Core/Utility/Utility.hpp"
#include "Core/Utility/Profiling.hpp"

#include "Core/GameTime.hpp"
#include "Input/Input.hpp"

#include "Core/EC/ArchetypeManager.hpp"

#include "Physics/PhysicsScene.hpp"
#include "Graphic/Opengl/RenderLoopOpengl.hpp"
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"
#include "Graphic/Opengl/Window.hpp"

#include "Graphic/RenderDoc/RenderDocManager.hpp"

using namespace Core;
using namespace Core::Container;
using namespace Core::ECS;

using namespace Graphic;
using namespace Physics;

namespace NightEngine2
{
  //Game Status Setting
  constexpr float      c_renderFPS = 60.0f;
  constexpr float      c_simulationFPS = 60.0f;
  constexpr float      c_TARGET_DT = 1.0f / c_renderFPS;
  constexpr float      c_AVR_FRAMERATE_SAMPLE = 15.0f;

  //TODO: there should be one per scene
  static PhysicsScene* g_physicScene = nullptr;

  Engine* Engine::s_instance = nullptr;

  void Engine::Initialize(void)
  {
    PROFILE_SESSION_BEGIN(nightengine2_profile_session_init);
    PROFILE_BLOCK_INSTRUMENT("NightEngine2::Initialize")
    {
      Debug::Log << "NightEngine2::Initialize\n";
      s_instance = this;

      m_gameTime = &(GameTime::GetInstance());
      *m_gameTime = GameTime{ c_renderFPS, c_simulationFPS, c_AVR_FRAMERATE_SAMPLE };
      m_gameTime->Subscribe(Core::MessageType::MSG_GAMESHOULDQUIT);

      //Physics
      g_physicScene = new PhysicsScene();

      //Core
      Reflection::Initialize();
      Factory::Initialize();
      ArchetypeManager::Initialize();

      //Runtime
      if (RenderDocManager::ShouldInitAtStartup())
      {
        RenderDocManager::Initialize();
      }

      //Initialize RenderLoop
      if (m_renderloop == nullptr)
      {
        Window::Initialize("NightEngine2", Window::WindowMode::WINDOW);
        m_renderloop = new RenderLoopOpengl();
        m_renderloop->Initialize();
      }
      
      Input::Initialize();

      //Physic Init After Graphic
      g_physicScene->Initialize();

      //TODO: Scene Init, Update, Terminate
    }
    PROFILE_SESSION_END();
  }

  void Engine::MainLoop(void)
  {
    while (!m_gameTime->m_shouldClose)
    {
      m_gameTime->StartFrame();
      PROFILE_BLOCK_INSTRUMENT("GameLoop")
      {
        float dt = m_gameTime->m_deltaTime;

        //Update Simulation
        PROFILE_BLOCK_INSTRUMENT("FixedUpdate")
        {
          FixedUpdate(dt);
        }

        PROFILE_BLOCK_INSTRUMENT("Update")
        {
          OnUpdate(dt);
        }

        //Render Frame
        RenderDocManager::StartFrameCapture();
        {
          PROFILE_BLOCK_INSTRUMENT("RenderLoop")
          {
            m_renderloop->Render(dt);
          }
        }
        RenderDocManager::EndFrameCapture(true);
      }
      PROFILE_BLOCK_INSTRUMENT("EndFrame")
      {
        m_gameTime->EndFrame();
      }

      //Defer the reinitialization to at the end of the frame
      if (m_reinitRenderLoop)
      {
        m_reinitRenderLoop = false;

        //Terminate RenderLoop
        if (m_renderloop != nullptr)
        {
          m_renderloop->Terminate();
          delete m_renderloop;
          m_renderloop = nullptr;

          Window::Terminate();
          OpenglAllocationTracker::PrintAllocationState();
          CHECKGL_ERROR();
        }
        RenderDocManager::Terminate();

        //Initialize RenderLoop
        if (m_renderloop == nullptr)
        {
          Window::Initialize("NightEngine2", Window::WindowMode::WINDOW);
          m_renderloop = new RenderLoopOpengl();
          m_renderloop->Initialize();
        }
      }
    }
  }

  void Engine::ReInitRenderLoop(void)
  {
    m_reinitRenderLoop = true;
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
  }

  void Engine::Terminate(void)
  {
    PROFILE_SESSION_BEGIN(nightengine2_profile_session_terminate);
    PROFILE_BLOCK_INSTRUMENT("NightEngine2::Terminate")
    {
      Debug::Log << "NightEngine2::Terminate\n";

      //Terminate System
      Input::Terminate();

      //Terminate RenderLoop
      if (m_renderloop != nullptr)
      {
        m_renderloop->Terminate();
        delete m_renderloop;
        m_renderloop = nullptr;

        //At this point, there should be no leaking gl object
        CHECKGL_ERROR();

        Window::Terminate();
        OpenglAllocationTracker::PrintAllocationState();
        CHECKGL_ERROR();
      }
      RenderDocManager::Terminate();

      ArchetypeManager::Terminate();
      Factory::Terminate();
      Reflection::Terminate();

      delete g_physicScene;

      m_gameTime->UnsubscribeAll();
    }
    PROFILE_SESSION_END();
  }

} // namespace World
