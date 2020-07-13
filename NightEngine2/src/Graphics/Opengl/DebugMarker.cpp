/*!
  @file DebugMarker.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Definition of DebugMarker
*/
#include "Graphics/Opengl/DebugMarker.hpp"
#include "Core/GameTime.hpp"

#include <glad/glad.h>

//Engine defines
#include "EngineConfigs.hpp"

namespace Rendering
{
  namespace DebugMarker
  {
    static int& GetID()
    {
      static int s_counter = 0;
      return s_counter;
    }

    void PushDebugGroup(const char* label, bool recordTimeStamp)
    {
#if(EDITOR_MODE)
      int& id = GetID();
      glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, (id), -1, label);
      NightEngine::GameTime::GetInstance().BeginTimeStamp(label, id);

      ++id;
#endif
    }

    void PopDebugGroup()
    {
#if(EDITOR_MODE)
      NightEngine::GameTime::GetInstance().EndTimeStamp();
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