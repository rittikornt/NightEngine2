/*!
  @file ConfirmationBox.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ConfirmationBox
*/
#pragma once
#include <string>

namespace Editor
{
  class ConfirmationBox
  {
  public:
    enum class BoxType
    {
      Normal = 0,
      Popup
    };
    enum class BoxField
    {
      Single = 0,
      Double,
      DoubleWithInput
    };
    using CallBack = void(*)(void);
    using CallBack1Param = void(*)(void*);

    //! @brief Constructor
    ConfirmationBox(void) {}

    //! @brief Constructor
    ConfirmationBox(BoxType type)
      : m_boxtype(type){}

    //! @brief Update ConfirmBox
    void Update(void);

    //! @brief Draw ConfirmationBox Field
    void DrawBoxField(void);

    //! @brief Show with description and set callback
    void ShowConfirmationBox(char* desc, CallBack callback
      , BoxField field = BoxField::Double);

    //! @brief Show with description and set callback
    void ShowConfirmationBoxWithInput(char* desc, CallBack1Param callback);
  private:
    bool        m_show = false;
    std::string m_windowName = "Confirmation Box";
    std::string m_text;
    char*       m_confirmationText = "";

    CallBack          m_callback = nullptr;
    CallBack1Param    m_callback1Param = nullptr;
    BoxType           m_boxtype = BoxType::Normal;
    BoxField          m_boxfield = BoxField::Double;

    char              m_inputBuffer[64] = "";
  };
  
}