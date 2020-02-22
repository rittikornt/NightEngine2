/*!
  @file RenderDocManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderDocManager
*/
#pragma once

namespace Rendering
{
  namespace RenderDocManager
  {
    bool ShouldInitAtStartup();

    bool Initialize();

    void Terminate();

    /////////////////////////////////////////////

    bool IsRenderDocAttached();

    //For wrapping capture boundary
    void StartFrameCapture();

    void EndFrameCapture(bool launchRenderDoc = false);

    //This is for triggering capture command
    void TriggerCapture();
  }
}
