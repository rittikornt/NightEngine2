/*!
  @file IRenderLoop.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderLoop
*/
#pragma once

namespace Rendering
{
  class IRenderLoop
  {
  public:
    virtual ~IRenderLoop() {}

    virtual void Initialize(void);

    virtual void Terminate(void);

    //TODO: remove float dt later, only here for debuging camera
    virtual void Render(float dt) = 0;

    virtual void OnRecompiledShader(void);
  };
} // Rendering

