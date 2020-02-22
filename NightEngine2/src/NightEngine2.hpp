/*!
  @file Engine.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Engine
*/
#pragma once

namespace Rendering
{
  class IRenderLoop;
}

namespace NightEngine
{
  class GameTime;
  
  enum class PostRenderEngineEvent
  {
    RestartWindow = 0,
    AttachRenderDoc,
    RecompileShader
  };

  class Engine
  {
  public:
    Engine(void) = default;
    ~Engine(void) = default;

    void Initialize(void);

    void Terminate(void);

    void MainLoop(void);

    void SendPostRenderEvent(PostRenderEngineEvent event);

    static Engine* GetInstance()
    {
      return s_instance;
    }

  private:
    void FixedUpdate(float dt);

    void OnUpdate(float dt);

    void ReInitRenderLoop_Internal(void);
  private:
    static Engine* s_instance;
    bool m_triggerPostRenderEvent = false;
    PostRenderEngineEvent m_event;

    bool m_shouldAttachRenderDoc = false;

    GameTime*   m_gameTime = nullptr;
    Rendering::IRenderLoop*   m_renderloop = nullptr;
  };

} // namespace NightEngine
