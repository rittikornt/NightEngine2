
/*!
  @file PostProcessSettingEditor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of PostProcessSettingEditor
*/

#include "Editor/PostProcessSettingEditor.hpp"
#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/GameObjectHierarchy.hpp"
#include "Editor/ConfirmationBox.hpp"
#include "imgui/imgui.h"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/ArchetypeManager.hpp"
#include "Core/EC/SceneManager.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

#include "Graphics/Opengl/Window.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"

using namespace NightEngine;
using namespace NightEngine::EC;
using namespace Rendering::Postprocess;
using namespace Rendering;
using namespace Reflection;

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);
  static ImVec4 g_color_orange_dark = ImVec4(0.96, 0.68f, 0.05f, 1.0f);
  static ImVec4 g_color_orange_light = ImVec4(0.96, 0.86f, 0.05f, 1.0f);
  static ImVec4 g_color_green = ImVec4(0.165f, 0.86f, 0.33f, 1.0f);

  //Confirmation Box
  static ConfirmationBox g_confirmBox{ConfirmationBox::BoxType::Popup };
  static std::string     g_confirmBoxDesc{ "" };

  //Component/GameObject Editor Shared States
  static PostProcessSetting* g_postprocessSetting;

  void PostProcessSettingEditor::Update(MemberSerializerEditor& memberSerializer)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer);
    }
    g_confirmBox.Update();
  }

  void PostProcessSettingEditor::Draw(bool* show, MemberSerializerEditor& memberSerializer)
  {
    g_postprocessSetting = &(SceneManager::GetPostProcessSetting());

    float width = 310.0f;
    float height = Window::GetHeight() * 0.5f;
    float topMenuHeight = 20.0f;
    ImGui::SetNextWindowPos(ImVec2(10, topMenuHeight + height), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(width, height - topMenuHeight), ImGuiCond_Appearing);
    
    //********************************************************
    // Window
    //********************************************************
    if (ImGui::Begin("PostProcessSettingEditor", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      // Component PostProcessSettingEditor
      ImGui::BeginGroup();
      {
        ImGui::BeginChild("GameObject Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          //GameObject Property, if not null
          if (g_postprocessSetting != nullptr)
          {
            auto& postProcessEffects = g_postprocessSetting->GetEffectsMetas();
            {
              ImGui::TextColored(g_color_orange_light
                , "PostProcessEffect Count: ");
              ImGui::SameLine();
              ImGui::Text("%d", postProcessEffects.size());
            }

            //All PostProcessEffects
            int nameMingle = 0;
            for (auto& effect : postProcessEffects)
            {
              //Traverse MetaType's Member
              auto metatype = effect.m_variable.GetMetaType();
              auto& typeName = metatype->GetName();

              ImGui::Columns(1, "Effect Header");
              if (ImGui::CollapsingHeader(typeName.c_str()))
              {
                ImGui::Checkbox((typeName + " enable").c_str(), &(effect.m_enable));

                //Column header
                ImGui::Columns(3, "Effect Property");
                ImGui::TextColored(g_color_green, "TYPE"); ImGui::NextColumn();
                ImGui::TextColored(g_color_green, "NAME"); ImGui::NextColumn();
                ImGui::TextColored(g_color_green, "VALUE"); ImGui::NextColumn();
                ImGui::Separator();

                //Members
                auto& members = metatype->GetMembers();
                for (auto& member : members)
                {
                  //TYPE
                  ImGui::TextColored(g_color_blue
                    , member.GetMetaType()->GetName().c_str());
                  ImGui::NextColumn();

                  //Name
                  ImGui::Text(member.GetName().c_str());
                  ImGui::NextColumn();

                  //Value Editor
                  memberSerializer.DrawMemberEditor(member
                    , effect.m_variable.GetValue(), std::to_string(nameMingle));
                  ImGui::NextColumn();
                  ImGui::Separator();
                }
              }
              ImGui::Separator();
              ++nameMingle;
            }

            //Clear Column
            ImGui::Columns(1);
          }
        }
        ImGui::EndChild();
      }
      ImGui::EndGroup();
    }
    ImGui::End();
  }

}