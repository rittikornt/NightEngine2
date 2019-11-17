/*!
  @file GraphicCore.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GraphicCore
*/
#pragma once

namespace Graphic
{
  void Initialize(void);

	//TODO: remove float dt later, only here for debuging camera
  void Render(float dt);

  void Terminate(void);

} // Graphic

