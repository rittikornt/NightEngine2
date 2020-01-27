/*!
  @file Engine.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Engine
*/
#pragma once

namespace Graphic
{
  class IRenderLoop;
}

namespace NightEngine2
{
  class GameTime;
  class Engine
  {
  public:
    Engine(void) = default;
    ~Engine(void) = default;

    void Initialize(void);

    void Terminate(void);

    void MainLoop(void);

    void ReInitRenderLoop(bool shouldAttachRenderDoc);

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
    bool m_reinitRenderLoop = false;
    bool m_shouldAttachRenderDoc = false;

    GameTime*   m_gameTime = nullptr;
    Graphic::IRenderLoop*   m_renderloop = nullptr;
  };

} // namespace NightEngine2
