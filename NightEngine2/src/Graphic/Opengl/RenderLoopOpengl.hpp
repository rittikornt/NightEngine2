/*!
  @file GraphicCore.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GraphicCore
*/
#pragma once

#include "Graphic/IRenderLoop.hpp"

namespace Graphic
{
  class RenderLoopOpengl: public IRenderLoop
  {
  public:
    virtual void Initialize(void) override;

    //TODO: remove float dt later, only here for debuging camera
    virtual void Render(float dt) override;

    virtual void Terminate(void) override;
  };
} // Graphic

