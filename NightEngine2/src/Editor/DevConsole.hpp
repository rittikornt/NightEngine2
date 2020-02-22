/*!
  @file DevConsole.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of DevConsole
*/
#pragma once
#include "imgui/imgui.h"

#include "Core/Message/IMessageHandler.hpp"   //Handle msg
#include "Core/Message/MessageObjectList.hpp" //Msg contents

#include <map>
#include <vector>

namespace Editor
{
  class DevConsole: public NightEngine::IMessageHandler
  {
  public:
    using CommandFn = void(DevConsole::*)(void);

    //! @brief Indicating which context the command is currently in
    enum class CommandContext : unsigned
    {
      COMMAND = 0
    };

    //! @brief Handle LogMessage
    virtual void HandleMessage(const NightEngine::LogMessage& msg);

    //! @brief Constructor
    DevConsole();

    //! @brief Destructor
    ~DevConsole();

    //! @brief Clear log
    void ClearLog();

    //! @brief AddCommand
    void AddCommand(const char* commandName, CommandFn commandFn);

    //! @brief Add log to console
    void AddLog(const char *fmt, ...) IM_FMTARGS(2)
    {
      // FIXME-OPT
      char buf[1024];
      va_list args;
      va_start(args, fmt);
      vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
      buf[IM_ARRAYSIZE(buf) - 1] = 0;
      va_end(args);
      m_Items.push_back(Strdup(buf));
      m_ScrollToBottom = true;
    }

    //! @brief Get reference to m_show
    bool& GetBool(void) { return m_show; }

    //! @brief Update
    void Update(void);

    //! @brief Draw Console
    void Draw(const char *title, bool *p_open, bool focus = false);

    //! @brief Handle command input
    void HandleInputCommand(char* inputText);

    //! @brief Handle Lua input
    void HandleInputLua(char* inputText);

    //! @brief Add log to console
    void ExecCommand(const char *command_line);

    //! @brief Save command history
    void SaveCommandHistory(const char *command_line);

    // In C++11 you are better off using lambdas for this sort of forwarding callbacks
    static int  TextEditCallbackStub(ImGuiTextEditCallbackData *data) ;
    int         TextEditCallback(ImGuiTextEditCallbackData *data);

    //*****************************************
    // CommandFunc
    //*****************************************
    void StartProfilingSession(void);
    void EndProfilingSession(void);
    void RenderDocCapture(void);
    void RestartWindow(void);
    void RecompileShaders(void);

    void ShowHelp(void);
    void ShowHistory(void);

    void RunUnitTest(void);

    //*****************************************
    // Helpers
    //*****************************************
    void         FindArguments(char* commandWithArgs);
    static int   Stricmp(const char *str1, const char *str2);
    static int   Strnicmp(const char *str1, const char *str2, int n);
    static char* Strdup(const char *str);
    static void  Strtrim(char *str);
  private:
    bool m_show = false;

    char m_InputBuf[256];
    ImVector<char *> m_Items;
    bool m_ScrollToBottom;
    ImVector<char *> m_History;
    int m_HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char *> m_Commands;

    CommandContext m_Context; //current context
    //Map for command
    std::map<std::string, CommandFn> m_CommandMap;
    std::vector<std::string> m_Arguments;

    //Color List
    ImColor m_Red{ 1.0f, 0.4f, 0.4f, 1.0f };
    ImColor m_Yellow{ 1.0f, 0.78f, 0.58f, 1.0f };
    ImColor m_Green{ 0.35f, 0.7f, 0.35f, 1.0f };
  };
}