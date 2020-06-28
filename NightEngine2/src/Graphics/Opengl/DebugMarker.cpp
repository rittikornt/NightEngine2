/*!
  @file DebugMarker.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Definition of DebugMarker
*/
#include "Graphics/Opengl/DebugMarker.hpp"

#include "Core/Macros.hpp"

#include <glad/glad.h>

namespace Rendering
{
  namespace DebugMarker
  {
    static int& GetID()
    {
      static int s_counter = 0;
      return s_counter;
    }

    void PushDebugGroup(const char* label)
    {
#if(EDITOR_MODE)
      glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, ++(GetID()), -1, label);
#endif
    }

    void PopDebugGroup()
    {
#if(EDITOR_MODE)
      glPopDebugGroup();
#endif
    }

    void EndFrame()
    {
      int& counter = GetID();
      counter = 0;
    }
  }
}