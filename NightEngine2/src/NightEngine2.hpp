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

  private:
    void FixedUpdate(float dt);

    void OnUpdate(float dt);

  private:
    GameTime*   m_gameTime = nullptr;
    Graphic::IRenderLoop*   m_renderloop = nullptr;
  };

} // namespace NightEngine2
