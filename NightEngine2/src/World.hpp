/*!
  @file World.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of World
*/
#pragma once

namespace World
{
  void Initialize(void);

  void MainLoop(void);

	void FixedUpdate(float dt);

  void OnUpdate(float dt);

  void Terminate(void);

} // namespace World
