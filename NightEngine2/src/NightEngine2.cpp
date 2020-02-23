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

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/ArchetypeManager.hpp"

#include "Physics/PhysicsScene.hpp"
#include "Graphics/RenderLoopOpengl.hpp"
#include "Graphics/Opengl/OpenglAllocationTracker.hpp"
#include "Graphics/Opengl/Window.hpp"
#include "Graphics/ShaderTracker.hpp"

#include "Graphics/RenderDoc/RenderDocManager.hpp"

using namespace NightEngine;
using namespace NightEngine::Container;
using namespace NightEngine::EC;

using namespace Rendering;
using namespace Physics;

namespace NightEngine
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
    PROFILE_BLOCK_INSTRUMENT("NightEngine::Initialize")
    {
      Debug::Log << "NightEngine::Initialize\n";
      s_instance = this;

      m_gameTime = &(GameTime::GetInstance());
      *m_gameTime = GameTime{ c_renderFPS, c_simulationFPS, c_AVR_FRAMERATE_SAMPLE };
      m_gameTime->Subscribe(NightEngine::MessageType::MSG_GAMESHOULDQUIT);

      //Physics
      g_physicScene = new PhysicsScene();

      //NightEngine
      Reflection::Initialize();
      Factory::Initialize();
      SceneManager::Initialize();
      ArchetypeManager::Initialize();

      //Runtime
      if (RenderDocManager::ShouldInitAtStartup())
      {
        RenderDocManager::Initialize();
      }

      //Initialize RenderLoop
      if (m_renderloop == nullptr)
      {
        Window::Initialize("NightEngine", Window::WindowMode::WINDOW);
        m_renderloop = new RenderLoopOpengl();
        m_renderloop->Initialize();
      }
      
      Input::Initialize();

      //Physic Init After Rendering
      g_physicScene->Initialize();

      //TODO: Scene Init, Update, Terminate
    }
    PROFILE_SESSION_END();
  }

  void Engine::Terminate(void)
  {
    PROFILE_SESSION_BEGIN(nightengine2_profile_session_terminate);
    PROFILE_BLOCK_INSTRUMENT("NightEngine::Terminate")
    {
      Debug::Log << "NightEngine::Terminate\n";

      //Terminate System
      Input::Terminate();

      //Terminate RenderLoop
      if (m_renderloop != nullptr)
      {
        m_renderloop->Terminate();
        delete m_renderloop;
        m_renderloop = nullptr;
      }
      RenderDocManager::Terminate();

      ArchetypeManager::Terminate();
      SceneManager::Terminate();
      Factory::Terminate();
      Reflection::Terminate();

      delete g_physicScene;

      m_gameTime->UnsubscribeAll();
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
      if (m_triggerPostRenderEvent)
      {
        m_triggerPostRenderEvent = false;

        switch (m_event)
        {
        case NightEngine::PostRenderEngineEvent::RestartWindow:
          m_shouldAttachRenderDoc = false;
          ReInitRenderLoop_Internal();
          break;
        case NightEngine::PostRenderEngineEvent::AttachRenderDoc:
          m_shouldAttachRenderDoc = true;
          ReInitRenderLoop_Internal();
          break;
        case NightEngine::PostRenderEngineEvent::RecompileShader:
          ShaderTracker::RecompileAllShaders();
          m_renderloop->OnRecompiledShader();
          break;
        }
      }
    }
  }

  ///////////////////////////////////////////////////////

  void Engine::SendPostRenderEvent(PostRenderEngineEvent event)
  {
    m_triggerPostRenderEvent = true;
    m_event = event;
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
      SceneManager::FixedUpdate();

      accumulator -= c_TARGET_DT;
    }
  }

  void Engine::OnUpdate(float dt)
  {
    //Debug::Log << "Engine::Update(" << g_gameStat->m_deltaTime
    //  << "), FPS: " << g_gameStat->m_frameRate << '\n';

    //Update Systems
    Input::OnUpdate();
    SceneManager::Update();

    //TODO: Update all the Components
  }

  void Engine::ReInitRenderLoop_Internal(void)
  {
    //Terminate RenderLoop
    if (m_renderloop != nullptr)
    {
      m_renderloop->Terminate();
      delete m_renderloop;
      m_renderloop = nullptr;
    }

    //Uninitialize RenderDoc if it is attached
    if (RenderDocManager::IsRenderDocAttached())
    {
      RenderDocManager::Terminate();
    }

    //Initialize RenderDoc if we need to
    if (m_shouldAttachRenderDoc)
    {
      ASSERT_MSG(RenderDocManager::Initialize(), "Failed to initialize RenderDocManager\n");
    }

    //Initialize RenderLoop
    if (m_renderloop == nullptr)
    {
      Window::Initialize("NightEngine", Window::WindowMode::WINDOW);
      m_renderloop = new RenderLoopOpengl();
      m_renderloop->Initialize();
      CHECKGL_ERROR();
    }
  }

} // namespace World
