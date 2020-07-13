/*!
  @file DebugMarker.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of DebugMarker
*/
#pragma once

namespace Rendering
{
  namespace DebugMarker
  {
    void PushDebugGroup(const char* label, bool recordTimeStamp = true);

    void PopDebugGroup();

    void EndFrame();
  }
}