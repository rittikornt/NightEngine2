
/*!
  @file ConfirmationBox.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ConfirmationBox
*/
#include "Editor/ConfirmationBox.hpp"
#include "imgui/imgui.h"
#include "Graphics/Opengl/Window.hpp"

#include "Core/Macros.hpp"

using namespace Rendering;

namespace Editor
{
  void ConfirmationBox::Update()
  {
    if (m_show)
    {
      switch (m_windowtype)
      {
        case Editor::ConfirmationBox::WindowType::Static:
        {
          auto size = ImVec2(300, 100);
          auto pos = ImVec2((Window::GetWidth()* 0.5f) - (size.x * 0.5f)
            , (Window::GetHeight()* 0.5f) - (size.y * 0.5f));
          ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
          ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);

          ImGui::Begin(m_windowName.c_str(), &m_show
            , ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
          {
            ImGui::TextWrapped(m_text.c_str());
            ImGui::Separator();

            DrawBoxContent();
            DrawConfirmOption();
          }
          ImGui::End();
          break;
        }
        case Editor::ConfirmationBox::WindowType::Popup:
        {
          auto size = ImVec2(300, 100);
          auto pos = ImVec2((Window::GetWidth()* 0.5f) - (size.x * 0.5f)
            , (Window::GetHeight()* 0.5f) - (size.y * 0.5f));
          ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
          ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);

          ImGui::OpenPopup(m_windowName.c_str());
          if (ImGui::BeginPopupModal(m_windowName.c_str(), &m_show
            , ImGuiWindowFlags_AlwaysAutoResize))
          {
            ImGui::TextWrapped(m_text.c_str());
            ImGui::Separator();

            DrawBoxContent();
            DrawConfirmOption();

            ImGui::EndPopup();
          }
          break;
        }
      }
    }
  }

  ////////////////////////////////////////////////////////

  void ConfirmationBox::ShowConfirmationBox(char * desc
    , CallBack callback, BoxConfirmOption confirmOption)
  {
    m_show = true;

    m_confirmationText = desc;
    m_text = "";
    m_text += desc;

    m_callback = callback;
    m_boxConfirmOption = confirmOption;
    m_boxContent = BoxContent::None;
    m_boxConfirmOption = BoxConfirmOption::Double;
  }

  void ConfirmationBox::ShowConfirmationBoxWithInput(char * desc
    , CallBack1Param callback)
  {
    m_show = true;

    m_confirmationText = desc;
    m_text = "";
    m_text += desc;

    m_callback1Param = callback;
    m_boxContent = BoxContent::TextInput;
    m_boxConfirmOption = BoxConfirmOption::DoubleWithCallback;
  }

  void ConfirmationBox::ShowConfirmationBoxWithComboBox(char* desc
    , CallBack1Param callback, std::vector<std::string>& items)
  {
    m_show = true;

    m_confirmationText = desc;
    m_text = "";
    m_text += desc;

    m_callback1Param = callback;
    m_boxContent = BoxContent::ComboBox;
    m_boxConfirmOption = BoxConfirmOption::DoubleWithCallback;

    m_items = items;
    m_itemsPtr.clear();
    for (int i=0; i < m_items.size(); ++i)
    {
      m_itemsPtr.emplace_back(m_items[i].c_str());
    }
  }

  bool ConfirmationBox::IsValidInput(void) const
  {
    switch (m_boxContent)
    {
      case BoxContent::TextInput:
      {
        return strlen(m_inputBuffer) != 0;
      }
      case BoxContent::ComboBox:
      {
        return m_currentItem < m_itemsPtr.size();
      }
    }

    return false;
  }

  ////////////////////////////////////////////////////////

  void ConfirmationBox::DrawConfirmOption(void)
  {
    switch (m_boxConfirmOption)
    {
    case BoxConfirmOption::Single:
    {
      ASSERT_TRUE(m_callback != nullptr);
      if (ImGui::Button("Ok", ImVec2(120, 0)))
      {
        m_show = false;
        if (m_windowtype == ConfirmationBox::WindowType::Popup)
        {
          ImGui::CloseCurrentPopup();
        }

        //Callback
        if (m_callback != nullptr)
        {
          m_callback();
        }
      }
      break;
    }
    case BoxConfirmOption::Double:
    {
      ASSERT_TRUE(m_callback != nullptr);
      if (ImGui::Button("Yes", ImVec2(120, 0)))
      {
        m_show = false;
        if (m_windowtype == ConfirmationBox::WindowType::Popup)
        {
          ImGui::CloseCurrentPopup();
        }

        //Callback
        if (m_callback != nullptr)
        {
          m_callback();
        }
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("No", ImVec2(120, 0)))
      {
        m_show = false;

        if (m_windowtype == ConfirmationBox::WindowType::Popup)
        {
          ImGui::CloseCurrentPopup();
        }
      }
      break;
    }
    case BoxConfirmOption::DoubleWithCallback:
    {
      ASSERT_TRUE(m_callback1Param != nullptr);

      //TODO: Can't click yes if buf is empty
      if (ImGui::Button("Yes", ImVec2(120, 0))
        && IsValidInput())
      {
        m_show = false;
        if (m_windowtype == ConfirmationBox::WindowType::Popup)
        {
          ImGui::CloseCurrentPopup();
        }

        //Callback
        if (m_callback1Param != nullptr)
        {
          m_callback1Param(m_inputBuffer);
          memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
        }
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("No", ImVec2(120, 0)))
      {
        m_show = false;

        if (m_windowtype == ConfirmationBox::WindowType::Popup)
        {
          ImGui::CloseCurrentPopup();
        }
      }
      break;
    }
    }
  }

  void ConfirmationBox::DrawBoxContent(void)
  {
    switch (m_boxContent)
    {
    case BoxContent::None:
    {
      break;
    }
    case BoxContent::TextInput:
    {
      ImGui::InputText("", m_inputBuffer, 64, ImGuiInputTextFlags_CharsNoBlank);
      break;
    }
    case BoxContent::ComboBox:
    {
      ImGui::Combo("select", &m_currentItem, m_itemsPtr.data(), m_itemsPtr.size());
      break;
    }
    }

  }
}