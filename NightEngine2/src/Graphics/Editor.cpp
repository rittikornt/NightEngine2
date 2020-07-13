/*!
  @file Editor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Editor
*/

#include "Editor.hpp"
#include <unordered_map>

//Rendering
#include "Graphics/Opengl/Window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

//TODO: Somehow make this Graphics API independent
#include "imgui/imgui_impl_opengl3.h"

//Editor
#include "Editor/PostProcessSettingEditor.hpp"
#include "Editor/DevConsole.hpp"

#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/ConfirmationBox.hpp"
#include "Editor/GameObjectBrowser.hpp"
#include "Editor/GameObjectHierarchy.hpp"
#include "Editor/ArchetypeBrowser.hpp"
#include "Editor/MaterialEditor.hpp"
#include "Editor/Inspector.hpp"

//Input
#include "Input/Input.hpp"

//NightEngine
#include "Core/Logger.hpp"
#include "Core/EC/SceneManager.hpp"
#include "Core/EC/Factory.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/GameTime.hpp"
#include "Core/Serialization/FileSystem.hpp"

//Math
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace NightEngine;
using namespace NightEngine::Factory;
using namespace Rendering;
using namespace NightEngine::EC;

namespace Editor
{
  //Editor
  static bool show_global_window = true;
  static bool show_frameinfo_window = true;

  //Top Menu
  static bool show_top_menu = true;

  //Custom editor
  static DevConsole g_devConsole;
  static PostProcessSettingEditor g_postprocessSetting;

  static MemberSerializerEditor g_memberSerializer;
  static GameObjectBrowser      g_gameObjectBrowser;
  static ArchetypeBrowser       g_archetypeBrowser;
  static MaterialEditor         g_materialEditor;

  static Inspector g_inspector;
  static Hierarchy g_hierarchy;

  //Confirmation box
  static ConfirmationBox g_confirmBox{ ConfirmationBox::WindowType::Popup};
  static ConfirmationBox g_comboBox{ ConfirmationBox::WindowType::Popup};

  /////////////////////////////////////////////////////////////////////////
  
  static bool show_demo_window = false;
  static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);//ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  static ImVec4 color_orange_dark = ImVec4(0.96, 0.68f, 0.05f, 1.0f);
  static ImVec4 color_orange_light = ImVec4(0.96, 0.86f, 0.05f, 1.0f);
  static ImVec4 color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);

  static ImVec4 color_green = ImVec4(0.165f, 0.86f, 0.33f, 1.0f);
  static ImVec4 color_red = ImVec4(0.658f, 0.196f, 0.274f, 1.0f);
  static ImVec4 color_yellow = ImVec4(0.909f, 0.9019f, 0.3804f, 1.0f);

  //********************************************
  // Utility
  //********************************************
  static void DrawOverlay(bool* p_open, char* header)
  {
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
    ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
    if (corner != -1)
      ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
    if (ImGui::Begin(header, p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
      ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
      ImGui::Separator();
      if (ImGui::IsMousePosValid())
        ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
      else
        ImGui::Text("Mouse Position: <invalid>");
      if (ImGui::BeginPopupContextWindow())
      {
        if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
        if (p_open && ImGui::MenuItem("Close")) *p_open = false;
        ImGui::EndPopup();
      }
      ImGui::End();
    }
  }

  static void ShowHelpMarker(const char* desc)
  {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
  }

  //********************************************
  // Top Mainmenu
  //********************************************
  static void DrawTopMenuFile()
  {
    ImGui::MenuItem("File menu", NULL, false, false);
    if (ImGui::MenuItem("New"))
    {
      SceneManager::CreateEmptyScene("New_Scene");
      Debug::Log << Logger::MessageType::INFO << "New Scene\n";
    }


    if (ImGui::MenuItem("Open", "Ctrl+O"))
    {
      static std::vector<std::string> s_paths;
      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Scenes, s_paths
      , FileSystem::FileFilter::FileName, ".nscene", true, false, true);

      g_comboBox.SetWindowName("Open Scene");
      g_comboBox.ShowConfirmationBoxWithComboBox("Open Scene: ",
        [](void* inputChars)
      {
        std::string sceneName = g_comboBox.GetSelectedComboBox();
        auto scenes = SceneManager::LoadScene(sceneName);
      }, s_paths);
    }
    /*if (ImGui::BeginMenu("Open Recent"))
    {
      ImGui::MenuItem("fish_hat.c");
      ImGui::MenuItem("fish_hat.inl");
      ImGui::MenuItem("fish_hat.h");
      if (ImGui::BeginMenu("More.."))
      {
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }*/
    if (ImGui::MenuItem("Save All", "Ctrl+S")) 
    {
      SceneManager::SaveAllScenes();
    }
    if (ImGui::MenuItem("Save As..")) 
    {
      g_confirmBox.SetWindowName("Save Scene As..");
      g_confirmBox.ShowConfirmationBoxWithInput("Save As: ",
        [](void* inputChars)
      {
        char* inputName = reinterpret_cast<char*>(inputChars);

        auto scenes = SceneManager::GetAllScenes();
        SceneManager::SaveSceneAs((*scenes)[0], inputName);
      });
    }
    ImGui::Separator();

    //Option
    if (ImGui::BeginMenu("Options"))
    {
      static bool enabled = true;
      ImGui::MenuItem("Enabled", "", &enabled);

      //Toggle resolution
      if (ImGui::Button("Toggle Resolution"))
      {
        Window::ToggleWindowSize();
      }
      ImGui::SameLine(0, 5);
      ImGui::Text("%d x %d", Window::GetWidth(), Window::GetHeight());

      //Toggle Fullscreen
      if (ImGui::Button("Toggle Fullscreen"))
      {
        Window::ToggleWindowMode();
      }
      ImGui::Separator();


      ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
          ImGui::Text("Scrolling Text %d", i);
      ImGui::EndChild();

      static float f = 0.5f;
      static int n = 0;
      static bool b = true;
      ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
      ImGui::InputFloat("Input", &f, 0.1f);
      ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
      ImGui::Checkbox("Check", &b);

      ImGui::EndMenu();
    }

    //Quit with confirm box
    if (ImGui::MenuItem("Quit", "EC")) 
    {
      g_confirmBox.ShowConfirmationBox("Are you sure you want to Quit", 
      [](void)
      {
        Window::SetWindowShouldClose(true);
      });
    }
  }

  static void DrawTopMainMenuBar()
  {
    if (ImGui::BeginMainMenuBar())
    {
      ImGui::Columns(6, "Column", false);
      //File
      if (ImGui::BeginMenu("File"))
      {
        DrawTopMenuFile();
        ImGui::EndMenu();
      }
      //Edit
      if (ImGui::BeginMenu("Edit"))
      {
        if (ImGui::MenuItem("Undo", "CTRL+Z")) 
        {
          Debug::Log << Logger::MessageType::INFO << "Undo\n";
        }
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))  // Disabled item
        {
          Debug::Log << Logger::MessageType::INFO << "Redo\n";
        }  
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        ImGui::EndMenu();
      }

      //Window
      if (ImGui::BeginMenu("Window"))
      {
        if (ImGui::MenuItem("Global Window", "", &show_global_window))
        {
          Debug::Log << Logger::MessageType::INFO
            << "Global Window: " << show_global_window << '\n';
        }
        if (ImGui::MenuItem("Profiling Window", "", &show_frameinfo_window))
        {
          Debug::Log << Logger::MessageType::INFO
            << "Profiling Window: " << show_frameinfo_window << '\n';
        }

        if (ImGui::MenuItem("DevConsole", "`",&g_devConsole.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO 
            << "DevConsole Window: " << g_devConsole.GetBool() << '\n';
        }
        if (ImGui::MenuItem("PostProcessSettings", "`", &g_postprocessSetting.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO
            << "PostProcessSettings Window: " << g_postprocessSetting.GetBool() << '\n';
        }

        if (ImGui::MenuItem("Hierachy", "", &g_hierarchy.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO 
            << "Hierachy Window: " << g_hierarchy.GetBool() << '\n';
        }
        if (ImGui::MenuItem("Inspector", "", &g_inspector.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO 
            << "Inspector Window: " << g_inspector.GetBool() << '\n';
        }
        if (ImGui::MenuItem("GameObject Browser", "", &g_gameObjectBrowser.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO
            << "GameObject Browser Window: " << g_gameObjectBrowser.GetBool() << '\n';
        }
        if (ImGui::MenuItem("Archetype Browser", "", &g_archetypeBrowser.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO
            << "Archetype Browser Window: " << g_archetypeBrowser.GetBool() << '\n';
        }
        if (ImGui::MenuItem("Material Editor", "", &g_materialEditor.GetBool()))
        {
          Debug::Log << Logger::MessageType::INFO
            << "Material Editor Window: " << g_materialEditor.GetBool() << '\n';
        }
        ImGui::EndMenu();
      }
      ImGui::Separator();

      //Next Column enough to push things to the right
      ImGui::NextColumn();
      ImGui::NextColumn();
      ImGui::NextColumn();
      ImGui::NextColumn();
      ImGui::NextColumn();

      //Fps debug text
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "fps: "); ImGui::SameLine(0, 5);
      
      auto fps = NightEngine::GameTime::GetInstance().m_averageFps;
      ImGui::Text("%.1f (%.3f ms)", fps, 1000.0f / fps); ImGui::SameLine(0, 5);
      
      //ImGui::Text("%.1f (%.3f ms)", ImGui::GetIO().Framerate,1000.0f / ImGui::GetIO().Framerate);
      ImGui::Separator();
      ShowHelpMarker("Average Frame Per Seconds");
      ImGui::Separator();

      ImGui::EndMainMenuBar();
    }
  }

  /////////////////////////////////////////////////////////////////////////

  static void RenderUI(void)
  {
    //Global window
    if (show_global_window)
    {
      ImGui::SetNextWindowBgAlpha(0.2f);
      ImGui::SetNextWindowSize(ImVec2(140, 160), ImGuiCond_Appearing);
      ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_Appearing);
      ImGui::Begin("Global Window", &show_global_window
      , ImGuiWindowFlags_AlwaysAutoResize);
      {
        ImGui::Separator();
        ImGui::Checkbox("Top Mainmenu", &show_top_menu);
        ImGui::Checkbox("DevConsole", &g_devConsole.GetBool());
        ImGui::Checkbox("Postprocess Settings", &g_postprocessSetting.GetBool());
        ImGui::Separator();
        ImGui::Checkbox("Hierarchy", &g_hierarchy.GetBool());
        ImGui::Checkbox("Inspector", &g_inspector.GetBool());
        ImGui::Checkbox("GameObject Browser", &g_gameObjectBrowser.GetBool() );
        ImGui::Checkbox("Archetype Browser", &g_archetypeBrowser.GetBool());
        ImGui::Checkbox("Material Editor", &g_materialEditor.GetBool());
        ImGui::Separator();

        ImGui::Checkbox("Demo Window", &show_demo_window);
      }
      ImGui::End();
    }

    //Global Profiling window
    if (show_frameinfo_window)
    {
      ImGui::SetNextWindowBgAlpha(0.4f);
      ImGui::SetNextWindowSize(ImVec2(140, 200), ImGuiCond_Appearing);
      ImGui::SetNextWindowPos(ImVec2(200, 20), ImGuiCond_Appearing);
      ImGui::Begin("Frame Information", &show_global_window
        , ImGuiWindowFlags_AlwaysAutoResize);
      {
        {
          //Helper lambda
          static auto GetTimeColor = [](float timeMS) -> ImVec4
          {
            const float cap_val = c_DEFAULT_FRAME_MS_CAP;
            const float cap_val_half = c_DEFAULT_FRAME_MS_CAP * 0.5f;
            return timeMS > cap_val ? color_red
              : timeMS > cap_val_half ? color_yellow : color_green;
          };

          static auto IMGUIIndent = [](int count)
          {
            bool unindent = count < 0;
            count = std::abs(count);
            for (int i = 0; i < count; ++i)
            {
              if (unindent)
              {
                ImGui::Unindent();
              }
              else
              {
                ImGui::Indent();
              }
            }
          };

          //Frame time Plot
          auto& gameTime = NightEngine::GameTime::GetInstance();
          char overlay[32];
          sprintf(overlay, "avr: %.3f ms", gameTime.m_averageFrameTimeMS);

          ImGui::PlotLines("", gameTime.m_averageFrameTimes.data()
            , gameTime.m_averageFrameTimes.size()
            , 0, overlay
            , 0, NightEngine::c_DEFAULT_FRAME_MS_CAP, ImVec2(0, 80));
          
          //Frame Time Texture
          auto totalFrameTimeMS = NightEngine::GameTime::GetInstance().m_currTotalFrameTimeMS.count();

          ImVec4 col = GetTimeColor(totalFrameTimeMS);
          ImGui::Text("Total Frame Time: "); ImGui::SameLine(0, 5);
          ImGui::TextColored(col, "(%.3f ms)", totalFrameTimeMS);
          ImGui::Separator();

          //Time Stamp texts
          int currIndentCount = 0;
          auto& timeStampResult = gameTime.m_frameTimeStampResult;
          std::string text = "";
          std::string timeText = "";
          for (auto& fts : timeStampResult)
          {
            text = fts.sortIndex == 0 ? "" : std::to_string(fts.sortIndex) + ") ";
            text += fts.name;
            text += ": ";

            timeText = "(%.3f ms)";
            col = GetTimeColor(fts.totalTimeMS);

            //Canceled the last indent, and set current indentation
            IMGUIIndent(-currIndentCount);
            IMGUIIndent(fts.indentCount);
            currIndentCount = fts.indentCount;

            ImGui::Text(text.c_str()); ImGui::SameLine(0, 5);
            ImGui::TextColored(col, timeText.c_str(), fts.totalTimeMS);
          }
        }

      }
      ImGui::End();
    }

    // Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). 
    if (show_demo_window)
    {
      ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_Appearing);
      ImGui::ShowDemoWindow(&show_demo_window);
    }

    //*********************************************
    //  Custom Editor
    //*********************************************
    //Update
    g_gameObjectBrowser.Update(g_memberSerializer);
    g_archetypeBrowser.Update(g_memberSerializer);
    g_materialEditor.Update(g_memberSerializer);
    g_hierarchy.Update();
    g_inspector.Update(g_memberSerializer, g_hierarchy);

    g_confirmBox.Update();
    g_comboBox.Update();
    g_devConsole.Update();
    g_postprocessSetting.Update(g_memberSerializer);

    //Top Menu
    if (show_top_menu)
    {
      DrawTopMainMenuBar();
    }
  }

  static void ProcessInput()
  {
    //Toggle dev console input
    if (Input::GetKeyDown(Input::KeyCode::KEY_GRAVE_ACCENT))
    {
      bool& show_dev_console = g_devConsole.GetBool();
      show_dev_console = !show_dev_console;
      if (show_dev_console)
      {
        g_devConsole.Draw("DevConsole", &show_dev_console, true);
      }
    }

    //Quit
    if (Input::GetKeyDown(Input::KeyCode::KEY_ESCAPE))
    {
      g_confirmBox.ShowConfirmationBox("Are you sure you want to Quit", [](void)
      {
        Window::SetWindowShouldClose(true);
      });
    }
  }

  /////////////////////////////////////////////////////////////////////////

  void Initialize(void)
  {
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(Rendering::Window::GetWindow(), true);
    ImGui_ImplOpenGL3_Init();

    // Setup style
    ImGui::StyleColorsDark();

    //TODO: Load all Material/Texture in the Folder
    /*static std::vector<std::string> s_matPaths;
    FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Materials , s_matPaths
      , FileSystem::FileFilter::FileName, ".mat", true);*/
  }

  void PreRender(void)
  {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Receive input
    ProcessInput();
    g_memberSerializer.NewFrame();
  }

  void PostRender(void)
  {
    RenderUI();

    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(Rendering::Window::GetWindow());
  }

  void Terminate(void)
  {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  /////////////////////////////////////////////////////////////////////////

} // Editor
