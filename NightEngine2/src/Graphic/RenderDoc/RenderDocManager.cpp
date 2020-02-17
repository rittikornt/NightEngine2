/*!
  @file RenderDocManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderDocManager
*/
#include "RenderDocManager.hpp"
#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "renderdoc/renderdoc_app.h"

#if _WIN32
  #include <Windows.h>
  #include "shellapi.h"
#endif

using namespace Core;

namespace Graphic
{
  namespace RenderDocManager
  {
    bool ShouldInitAtStartup()
    {
      //NOTE: Modify this to Attach RenderDoc at startup or not
      return false;
    }

    //Only working in Windows debug mode
#if DEBUG_MODE && _WIN32
    static bool g_attached = false;
    static bool g_triggerCapture = false;
    static bool g_capturing = false;

    static RENDERDOC_API_1_4_0 *g_renderDocAPI = NULL;
    static HMODULE g_renderDocDLL = nullptr;
    static bool g_dllLoadedManually = false;

    static const char* g_renderDocEXEPath = "C:\\Program Files\\RenderDoc\\qrenderdoc.exe";
    static const char* g_renderDocDLLPath = "C:\\Program Files\\RenderDoc\\renderdoc.dll";

    bool Initialize()
    {
      Debug::Log << "RenderDocManager: Initialize\n";

      if (g_attached)
      {
        Debug::Log << Core::Logger::MessageType::WARNING
          << "RenderDoc Already Attached (Attempting to Intialize RenderDocManager)\n";
        return false;
      }

      //If launch by RenderDoc, this should not be null
      g_renderDocDLL = GetModuleHandle("renderdoc.dll");
      if (g_renderDocDLL == nullptr)
      {
        //Load library manually if not launch by RenderDoc
        g_renderDocDLL = LoadLibrary(g_renderDocDLLPath);
        g_dllLoadedManually = true;
      }

      if (g_renderDocDLL != nullptr)
      {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(g_renderDocDLL, "RENDERDOC_GetAPI");
        int result = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void **)&g_renderDocAPI);

        g_renderDocAPI->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
        g_renderDocAPI->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, 1);

        if (result)
        {
          Debug::Log << "RenderDoc Attached: Succesfully\n";
          g_attached = true;
        }
        else
        {
          Debug::Log << Core::Logger::MessageType::ERROR_MSG
            << "RenderDoc Attached: Failed\n";
          g_attached = false;
        }
      }
      else
      {
        Debug::Log << Core::Logger::MessageType::WARNING
          << "RenderDoc Attached: Failed (renderdoc.dll not found)\n";
        g_attached = false;
      }

      return g_attached;
    }

    void Terminate()
    {
      Debug::Log << "RenderDocManager: Terminate\n";

      if (g_renderDocAPI != nullptr)
      {
        g_renderDocAPI->Shutdown();
      }
      g_renderDocAPI = nullptr;

      //If loaded manually, it must be free
      if (g_dllLoadedManually)
      {
        auto result = FreeLibrary(g_renderDocDLL);;
      }

      g_attached = false;
    }

    bool IsRenderDocAttached()
    {
      return g_attached;
    }

    void StartFrameCapture()
    {
      if (!g_capturing 
        && g_triggerCapture
        && g_renderDocAPI != nullptr)
      {
        Debug::Log << "RenderDocManager: StartFrameCapture\n";
        g_renderDocAPI->StartFrameCapture(NULL, NULL);

        g_capturing = true;
        g_triggerCapture = false;
      }
    }

    void EndFrameCapture(bool launchRenderDoc)
    {
      if (g_capturing && g_renderDocAPI != nullptr)
      {
        Debug::Log << "RenderDocManager: EndFrameCapture\n";
        g_renderDocAPI->EndFrameCapture(NULL, NULL);

        g_capturing = false;

        const auto numCapture = g_renderDocAPI->GetNumCaptures();
        if (numCapture > 0)
        {
          unsigned int pathLen;
          g_renderDocAPI->GetCapture(numCapture - 1, nullptr, &pathLen, nullptr);
          
          //TODO: use a fixed size char[] instead of newing here
          char* filename = new char[pathLen];
          {
            g_renderDocAPI->GetCapture(numCapture - 1, filename, nullptr, nullptr);

            if (launchRenderDoc)
            {
              // TODO: we might be able to somehow attach the renderdoc to NightEngine2 process
              //  with some RenderDoc commandline arguments
              auto renderdoc_exe = ShellExecute(nullptr, "open"
                , g_renderDocEXEPath, filename, nullptr, true);
            }
          }
          Debug::Log << "Total Captured Frame: " << numCapture 
            << " [" << filename << "]\n";
          delete[] filename;
        }
      }
    }
    
    void TriggerCapture()
    {
      Debug::Log << "RenderDocManager: TriggerCapture\n";
      
      g_triggerCapture = true;
    }
#else
    bool Initialize()
    {
    }

    void Terminate()
    {
    }

    void StartFrameCapture()
    {
    }

    void EndFrameCapture(bool launchRenderDoc)
    {
    }

    void TriggerCapture()
    {
    }

    bool IsRenderDocAttached()
    {
      return false;
    }
#endif
  }
}
