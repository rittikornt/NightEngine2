/*!
  @file ConfirmationBox.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ConfirmationBox
*/
#pragma once
#include <string>
#include <vector>

namespace Editor
{
  class ConfirmationBox
  {
  public:
    enum class WindowType
    {
      Static = 0,
      Popup
    };
    enum class BoxContent
    {
      None = 0,
      TextInput,
      ComboBox
    };
    enum class BoxConfirmOption
    {
      Single = 0,
      Double,
      DoubleWithCallback
    };
    using CallBack = void(*)(void);
    using CallBack1Param = void(*)(void*);

    //! @brief Constructor
    ConfirmationBox(void) {}

    //! @brief Constructor
    ConfirmationBox(WindowType type)
      : m_windowtype(type){}

    //! @brief Update ConfirmBox
    void Update(void);

    //! @brief Show with description and set callback
    void ShowConfirmationBox(char* desc, CallBack callback
      , BoxConfirmOption field = BoxConfirmOption::Double);

    //! @brief Show with description and set callback
    void ShowConfirmationBoxWithInput(char* desc, CallBack1Param callback);

    //! @brief Show with description and set callback
    void ShowConfirmationBoxWithComboBox(char* desc, CallBack1Param callback
      , std::vector<std::string>& items);

    //! @brief Check if we have valid input
    bool IsValidInput(void) const;

    //! @brief Set the window name
    inline void SetWindowName(const char* windowName) { m_windowName = windowName; }

    //! @brief Selected Combobox
    inline const char* GetSelectedComboBox(void) const { return m_itemsPtr[m_currentItem]; }
  private:
    //! @brief Draw ConfirmOption
    void DrawConfirmOption(void);

    //! @brief Draw BoxContent
    void DrawBoxContent(void);

    bool                 m_show = false;
    std::string          m_windowName = "Confirmation Box";
    std::string          m_text;
    char*                m_confirmationText = "";

    CallBack             m_callback = nullptr;
    CallBack1Param       m_callback1Param = nullptr;

    WindowType           m_windowtype = WindowType::Static;
    BoxContent           m_boxContent = BoxContent::None;
    BoxConfirmOption     m_boxConfirmOption = BoxConfirmOption::Double;
    char                 m_inputBuffer[128] = "";

    //Combo Box
    std::vector <std::string> m_items;
    std::vector <const char*> m_itemsPtr;
    int m_currentItem = 0;
  };
  
}