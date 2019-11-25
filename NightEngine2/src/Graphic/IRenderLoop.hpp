/*!
  @file IRenderLoop.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderLoop
*/
#pragma once

namespace Graphic
{
  class IRenderLoop
  {
  public:
    virtual void Initialize(void);

    //TODO: remove float dt later, only here for debuging camera
    virtual void Render(float dt) = 0;

    virtual void Terminate(void);
  };
} // Graphic

