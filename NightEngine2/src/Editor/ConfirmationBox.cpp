
/*!
  @file ConfirmationBox.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ConfirmationBox
*/
#include "Editor/ConfirmationBox.hpp"
#include "imgui/imgui.h"
#include "Graphic/Opengl/Window.hpp"

#include "Core/Macros.hpp"

using namespace Graphic;

namespace Editor
{
  void ConfirmationBox::Update()
  {
    if (m_show)
    {
      switch (m_boxtype)
      {
        case Editor::ConfirmationBox::BoxType::Normal:
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

            DrawBoxField();
          }
          ImGui::End();
          break;
        }
        case Editor::ConfirmationBox::BoxType::Popup:
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

            DrawBoxField();

            ImGui::EndPopup();
          }
          break;
        }
      }
    }
  }

  void ConfirmationBox::DrawBoxField(void)
  {
    switch (m_boxfield)
    {
      case BoxField::Single:
      {
        ASSERT_TRUE(m_callback != nullptr);
        if (ImGui::Button("Ok", ImVec2(120, 0)))
        {
          m_show = false;
          if (m_boxtype == ConfirmationBox::BoxType::Popup)
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
      case BoxField::Double:
      {
        ASSERT_TRUE(m_callback != nullptr);
        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
          m_show = false;
          if (m_boxtype == ConfirmationBox::BoxType::Popup)
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

          if (m_boxtype == ConfirmationBox::BoxType::Popup)
          {
            ImGui::CloseCurrentPopup();
          }
        }
        break;
      }
      case BoxField::DoubleWithInput:
      {
        ASSERT_TRUE(m_callback1Param != nullptr);

        ImGui::InputText("", m_inputBuffer, 64, ImGuiInputTextFlags_CharsNoBlank);

        //TODO: Can't click yes if buf is empty
        if (ImGui::Button("Yes", ImVec2(120, 0))
          && strlen(m_inputBuffer) != 0)
        {
          m_show = false;
          if (m_boxtype == ConfirmationBox::BoxType::Popup)
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

          if (m_boxtype == ConfirmationBox::BoxType::Popup)
          {
            ImGui::CloseCurrentPopup();
          }
        }
        break;
      }
    }

  }

  void ConfirmationBox::ShowConfirmationBox(char * desc
    , CallBack callback, BoxField field)
  {
    m_show = true;

    m_confirmationText = desc;
    m_text = "Are you sure you want to ";
    m_text += desc;

    m_callback = callback;
    m_boxfield = field;
  }

  void ConfirmationBox::ShowConfirmationBoxWithInput(char * desc
    , CallBack1Param callback)
  {
    m_show = true;

    m_confirmationText = desc;
    m_text = "Are you sure you want to ";
    m_text += desc;

    m_callback1Param = callback;
    m_boxfield = BoxField::DoubleWithInput;
  }
}