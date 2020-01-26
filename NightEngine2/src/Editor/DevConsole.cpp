/*!
  @file DevConsole.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of DevConsole
*/

#pragma once
#include "Editor/DevConsole.hpp"

//#include "Core/Lua/LuaCore.hpp"
#include "UnitTest/UnitTest.hpp"

#include "Graphic/Opengl/Window.hpp"
#include "Graphic/RenderDoc/RenderDocManager.hpp"

#include "Core/Utility/Profiling.hpp"
#include "Core/Serialization/FileSystem.hpp"

#include "NightEngine2.hpp"

#include <ctype.h>          // toupper, isprint
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#include <stdint.h>         // intptr_t
#include <string>

namespace Editor
{
  //***************************************
  // Global
  //***************************************

  //***************************************
  // Definition
  //***************************************
  void DevConsole::HandleMessage(const Core::LogMessage& msg)
  {
    AddLog(msg.m_string.c_str());
  }

  DevConsole::DevConsole()
  {
    ClearLog();
    memset(m_InputBuf, 0, sizeof(m_InputBuf));
    m_HistoryPos = -1;
    m_Context = CommandContext::COMMAND;

    //Add command
    AddCommand("HELP", &DevConsole::ShowHelp);
    AddCommand("HISTORY", &DevConsole::ShowHistory);
    AddCommand("CLEAR", &DevConsole::ClearLog);
    AddCommand("UNITTEST", &DevConsole::RunUnitTest);

    AddCommand("BEGINPROFILE", &DevConsole::StartProfilingSession);
    AddCommand("ENDPROFILE", &DevConsole::EndProfilingSession);

    AddCommand("RENDERDOC_CAPTURE", &DevConsole::RenderDocCapture);
    AddCommand("RESTART", &DevConsole::ReinitEngine);

    //Init Log
    AddLog("//******************************");
    AddLog("// DevConsole Start!");
    AddLog("//******************************\n\n");

    //To receive log from Logger
    using namespace Core;
    Subscribe("MSG_LOGMESSAGE");
  }

  DevConsole::~DevConsole()
  {
    ClearLog();
    for (int i = 0; i < m_History.Size; i++)
      free(m_History[i]);
  }

  void DevConsole::ClearLog()
  {
    for (int i = 0; i < m_Items.Size; i++)
      free(m_Items[i]);
    m_Items.clear();
    m_ScrollToBottom = true;
  }

  void DevConsole::AddCommand(const char* commandName, CommandFn commandFn)
  {
    m_Commands.push_back(commandName);
    m_CommandMap.insert({ commandName,commandFn });
  }

  void DevConsole::Update(void)
  {
    if (m_show)
    {
      Draw("DevConsole", &m_show);
    }
  }

  void DevConsole::Draw(const char *title, bool *p_open, bool focus)
  {
    ImGui::SetNextWindowPos(ImVec2(5, Graphic::Window::GetHeight() - 305), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_Appearing);
    if (!ImGui::Begin(title, p_open))
    {
      ImGui::End();
      return;
    }

    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
      if (ImGui::MenuItem("Close"))
        *p_open = false;
      ImGui::EndPopup();
    }

    //ImGui::TextWrapped("This example implements a console with basic coloring, completion and history. A more elaborate implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.");
    
    // TODO: display items starting from the bottom

    /*if (ImGui::SmallButton("Add Dummy Text"))
    {
      AddLog("%d some text", m_Items.Size);
      AddLog("some more text");
      AddLog("display very important message here!");
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add Dummy Error"))
    {
      AddLog("[error] something went wrong");
    }
    ImGui::SameLine();*/
    if (ImGui::SmallButton("Clear"))
    {
      ClearLog();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    ImGui::SameLine();
    if (ImGui::SmallButton("Scroll to bottom"))
      m_ScrollToBottom = true;
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    static ImGuiTextFilter filter;
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::PopStyleVar();
    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();             // 1 separator, 1 input text
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
    if (ImGui::BeginPopupContextWindow())
    {
      if (ImGui::Selectable("Clear"))
        ClearLog();
      ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
    // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
    // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
    //     ImGuiListClipper clipper(Items.Size);
    //     while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
    // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
    // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
    // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
      ImGui::LogToClipboard();
    ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    for (int i = 0; i < m_Items.Size; i++)
    {
      const char *item = m_Items[i];
      if (!filter.PassFilter(item))
        continue;

      //Color Filter
      ImVec4 col = col_default_text;
      if (strstr(item, "[error]"))
      {
        col = m_Red;
      }
      else if (strstr(item, "[warning]")
        || strncmp(item, "# ", 2) == 0)
      {
        col = m_Yellow;
      }
      else if (strstr(item, "[info]") 
        || strstr(item, "[lua]")
        || strstr(item, "//"))
      {
        col = m_Green;
      }
      ImGui::PushStyleColor(ImGuiCol_Text, col);
      ImGui::TextUnformatted(item);
      ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
      ImGui::LogFinish();
    if (m_ScrollToBottom)
      ImGui::SetScrollHere(1.0f);
    m_ScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = focus;
    if (ImGui::InputText("Input", m_InputBuf, IM_ARRAYSIZE(m_InputBuf)
      , ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory
      , &TextEditCallbackStub, (void *)this))
    {
      switch (m_Context)
      {
        case CommandContext::COMMAND:
        {
          HandleInputCommand(m_InputBuf);
          break;
        }
      }

      reclaim_focus = true;
    }

    // Demonstrate keeping focus on the input box
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
    {
      ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();
  }

  void DevConsole::HandleInputCommand(char* inputText)
  {
    //Save history + arguments
    SaveCommandHistory(inputText);
    FindArguments(inputText);

    //Trim and execute command
    Strtrim(inputText);
    if (inputText[0])
    {
      ExecCommand(inputText);
    }
    strcpy(inputText, "");
  }

  void DevConsole::HandleInputLua(char* inputText)
  {
    SaveCommandHistory(inputText);

    //Only look for luaend command
    if (Stricmp(inputText, "luaend") == 0)
    {
      ExecCommand(inputText);
      strcpy(inputText, "");
      return;
    }

    //Keep lua script to the Argument string
    if (m_Arguments.size() == 0)
    {
      m_Arguments.emplace_back(inputText);
    }
    else
    {
      m_Arguments[0].append("\n");
      m_Arguments[0].append(inputText);
    }
    AddLog(inputText);
    strcpy(inputText, "");
  }

  void DevConsole::ExecCommand(const char *command_line)
  {
    AddLog("# %s\n", command_line);

    // Convert command to Upper then Process the command
    std::string commandStr{ command_line };
    for (auto & c : commandStr)
    {
      c = toupper(c);
    }

    //Lookup command
    auto it = m_CommandMap.find(commandStr);
    if (it != m_CommandMap.end())
    {
      (this->*(it->second))();
    }
    else
    {
      AddLog("Unknown command: '%s'\n", command_line);
    }
  }

  void DevConsole::SaveCommandHistory(const char *command_line)
  {
    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    m_HistoryPos = -1;
    for (int i = m_History.Size - 1; i >= 0; i--)
      if (Stricmp(m_History[i], command_line) == 0)
      {
        free(m_History[i]);
        m_History.erase(m_History.begin() + i);
        break;
      }
    m_History.push_back(Strdup(command_line));
  }

  int DevConsole::TextEditCallbackStub(ImGuiTextEditCallbackData *data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
  {
    DevConsole *console = (DevConsole *)data->UserData;
    return console->TextEditCallback(data);
  }

  int DevConsole::TextEditCallback(ImGuiTextEditCallbackData *data)
  {
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
      case ImGuiInputTextFlags_CallbackCompletion:
      {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char *word_end = data->Buf + data->CursorPos;
        const char *word_start = word_end;
        while (word_start > data->Buf)
        {
          const char c = word_start[-1];
          if (c == ' ' || c == '\t' || c == ',' || c == ';')
            break;
          word_start--;
        }

        // Build a list of candidates
        ImVector<const char *> candidates;
        for (int i = 0; i < m_Commands.Size; i++)
        {
          if (Strnicmp(m_Commands[i], word_start, (int)(word_end - word_start)) == 0)
            candidates.push_back(m_Commands[i]);
        }

        if (candidates.Size == 0)
        {
          // No match
          AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        }
        else if (candidates.Size == 1)
        {
          // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
          data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0]);
          data->InsertChars(data->CursorPos, " ");
        }
        else
        {
          // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
          int match_len = (int)(word_end - word_start);
          for (;;)
          {
            int c = 0;
            bool all_candidates_matches = true;
            for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
              if (i == 0)
                c = toupper(candidates[i][match_len]);
              else if (c == 0 || c != toupper(candidates[i][match_len]))
                all_candidates_matches = false;
            if (!all_candidates_matches)
              break;
            match_len++;
          }

          if (match_len > 0)
          {
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
          }

          // List matches
          AddLog("Possible matches:\n");
          for (int i = 0; i < candidates.Size; i++)
            AddLog("- %s\n", candidates[i]);
        }

        break;
      }
      case ImGuiInputTextFlags_CallbackHistory:
      {
        // Example of HISTORY
        const int prev_history_pos = m_HistoryPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
          if (m_HistoryPos == -1)
            m_HistoryPos = m_History.Size - 1;
          else if (m_HistoryPos > 0)
            m_HistoryPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
          if (m_HistoryPos != -1)
            if (++m_HistoryPos >= m_History.Size)
              m_HistoryPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (prev_history_pos != m_HistoryPos)
        {
          data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (m_HistoryPos >= 0) ? m_History[m_HistoryPos] : "");
          data->BufDirty = true;
        }
      }
    }
    return 0;
  }

  //***************************************
  // CommandFunc
  //***************************************

  void DevConsole::StartProfilingSession(void)
  {
    PROFILE_SESSION_BEGIN(nightengine2_profile_session_runtime);
  }

  void DevConsole::EndProfilingSession(void)
  {
    PROFILE_SESSION_END();
  }

  void DevConsole::RenderDocCapture(void)
  {
    using namespace Graphic;
   
    if (RenderDocManager::IsRenderDocAttached())
    {
      RenderDocManager::TriggerCapture();
    }
    else
    {
      //Try to Initialize before trigger capture
      if (RenderDocManager::Initialize())
      {
        //RenderDocManager::TriggerCapture();
        //TODO: in order for this to work, we need to reinit the window
        NightEngine2::Engine::GetInstance()->ReInitRenderLoop();
      }
    }
  }

  void DevConsole::ReinitEngine(void)
  {
    using namespace Graphic;

    //TODO: in order for this to work, we need to reinit the window
    NightEngine2::Engine::GetInstance()->ReInitRenderLoop();
  }

  void DevConsole::ShowHelp(void)
  {
    AddLog("Commands:");
    for (int i = 0; i < m_Commands.Size; i++)
      AddLog("- %s", m_Commands[i]);
  }

  void DevConsole::ShowHistory(void)
  {
    int first = m_History.Size - 10;
    for (int i = first > 0 ? first : 0; i < m_History.Size; i++)
      AddLog("%3d: %s\n", i, m_History[i]);
  }

  void DevConsole::RunUnitTest(void)
  {
    //Convert to vector of char*
    std::vector<char*> argv{ { "Game.exe" } };
    argv.reserve(m_Arguments.size() + 1);
    for (auto it = m_Arguments.rbegin(); it != m_Arguments.rend()
      ; ++it)
    {
      argv.emplace_back( &((*it)[0]) );
    }

    UnitTest::RunTest(argv);
  }

  //***************************************
  // Helpers
  //***************************************

  //! @brief Find Arguments from a ' ' separated char* and add it to m_Arguments
  void DevConsole::FindArguments(char* commandWithArgs)
  {
    //Clear Arguments
    m_Arguments.clear();

    int delimitterCount = 0;
    //Point to last char and Traverse back to the front
    char *str_end = commandWithArgs + strlen(commandWithArgs);
    while (str_end > commandWithArgs)
    {
      //Ignore space within ()
      if (str_end[-1] == ')')
      {
        ++delimitterCount;
      }
      else if (str_end[-1] == '(')
      {
        --delimitterCount;
      }

      //Found space separated char*
      if (str_end[-1] == ' ' && delimitterCount == 0)
      {
        m_Arguments.emplace_back(str_end);
        str_end[-1] = 0;
      }

      --str_end;
    }
  }

  int DevConsole::Stricmp(const char *str1, const char *str2)
  {
    int d;
    while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
    {
      str1++;
      str2++;
    }
    return d;
  }
  int DevConsole::Strnicmp(const char *str1, const char *str2, int n)
  {
    int d = 0;
    while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1)
    {
      str1++;
      str2++;
      n--;
    }
    return d;
  }
  char* DevConsole::Strdup(const char *str)
  {
    size_t len = strlen(str) + 1;
    void *buff = malloc(len);
    return (char *)memcpy(buff, (const void *)str, len);
  }
  void DevConsole::Strtrim(char *str)
  {
    char *str_end = str + strlen(str);
    while (str_end > str && str_end[-1] == ' ')
      str_end--;
    *str_end = 0;
  }

}