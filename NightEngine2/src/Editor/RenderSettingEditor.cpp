
/*!
  @file RenderSettingEditor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderSettingEditor
*/

#include "Editor/RenderSettingEditor.hpp"
#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/GameObjectHierarchy.hpp"
#include "Editor/ConfirmationBox.hpp"
#include "imgui/imgui.h"

#include "NightEngine2.hpp"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/ArchetypeManager.hpp"
#include "Core/EC/SceneManager.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

#include "Graphics/Opengl/Window.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"
#include "Graphics/IRenderLoop.hpp"
#include "Graphics/RenderLoopOpengl.hpp"

using namespace NightEngine;
using namespace NightEngine::EC;
using namespace NightEngine::Rendering::Opengl::Postprocess;
using namespace NightEngine::Rendering::Opengl;
using namespace NightEngine::Rendering;
using namespace Reflection;

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);
  static ImVec4 g_color_orange_dark = ImVec4(0.96, 0.68f, 0.05f, 1.0f);
  static ImVec4 g_color_orange_light = ImVec4(0.96, 0.86f, 0.05f, 1.0f);
  static ImVec4 g_color_green = ImVec4(0.165f, 0.86f, 0.33f, 1.0f);

  //Confirmation Box
  static ConfirmationBox g_confirmBox{ ConfirmationBox::WindowType::Popup };
  static std::string     g_confirmBoxDesc{ "" };

  //Component/GameObject Editor Shared States
  static PostProcessSetting* g_postprocessSetting;

  void RenderSettingEditor::Update(MemberSerializerEditor& memberSerializer)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer);
    }
    g_confirmBox.Update();
  }

  void RenderSettingEditor::Draw(bool* show, MemberSerializerEditor& memberSerializer)
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
    if (ImGui::Begin("Render Settings", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      int treeNodeFlag = ImGuiTreeNodeFlags_DefaultOpen;

      auto engine = NightEngine::Engine::GetInstance();
      auto irenderLoop = engine->GetRenderLoop();
      RenderLoopOpengl* rlgl = irenderLoop->GetAPI() == GraphicsAPI::OPENGL ?
        static_cast<RenderLoopOpengl*>(irenderLoop) : nullptr;

      // Component PostProcessSettingEditor
      ImGui::BeginGroup();
      {
        ImGui::BeginChild("Rendering Settings Headers", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          //GameObject Property, if not null
          if (g_postprocessSetting != nullptr
            && ImGui::CollapsingHeader("PostProcess Settings", treeNodeFlag))
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
            Variable tempVar{ METATYPE_FROM_STRING("int"),nullptr };
            for (auto& effect : postProcessEffects)
            {
              //Traverse MetaType's Member
              auto metatype = effect->m_metaType;// m_variable.GetMetaType();
              auto& typeName = metatype->GetName();

              ImGui::Columns(1, "Effect Header");
              if (ImGui::CollapsingHeader(typeName.c_str()))
              {
                ImGui::Checkbox((typeName + " enable").c_str(), &(effect->m_enable));

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
                  effect->GetVariable(tempVar);
                  memberSerializer.DrawMemberEditor(member
                    , tempVar.GetValue(), std::to_string(nameMingle));
                  ImGui::NextColumn();
                }
              }
              ++nameMingle;
            }

            //Clear Column
            ImGui::Columns(1);
          }
          ImGui::Separator();

          //Debug View ComboBox
          if (ImGui::CollapsingHeader("Debug View Settings", treeNodeFlag))
          {
            ImGui::Indent();
            {
              static int s_debugViewEnum = 0;
              static int s_debugShadowViewEnum = 0;

              bool changed = ImGui::Combo("Debug View", &s_debugViewEnum
                , k_debugViewStr, (int)NightEngine::Rendering::DebugView::COUNT);

              changed |= ImGui::Combo("Shadow Debug View", &s_debugShadowViewEnum
                , k_debugShadowViewStr, (int)NightEngine::Rendering::DebugShadowView::COUNT);

              static float s_zoom = 1.0f;
              if (ImGui::SliderFloat("Zoom", &s_zoom, 1.0f, 10.0f))
              {
                if (rlgl != nullptr)
                {
                  rlgl->screenZoomScale = s_zoom;
                }
              }

              if (changed)
              {
                NightEngine::Rendering::DebugView dv = (NightEngine::Rendering::DebugView)s_debugViewEnum;
                NightEngine::Rendering::DebugShadowView dsv = (NightEngine::Rendering::DebugShadowView)s_debugShadowViewEnum;
                engine->GetRenderLoop()->SetDebugViews(dv, dsv);
              }
            }
            ImGui::Unindent();
          }

          if (rlgl != nullptr)
          {
            if (ImGui::CollapsingHeader("Lighting Settings", treeNodeFlag))
            {
              ImGui::Indent();
              {
                static float s_ai_min = 0.0f;
                static float s_ai_max = 1.0f;
                ImGui::DragScalar("Ambient Intensity", ImGuiDataType_Float
                  , &(rlgl->ambientStrength), 0.1f, &s_ai_min, &s_ai_max);

                static float s_rs_min = 0.1f;
                static float s_rs_max = 2.0f;
                ImGui::DragScalar("Render Scale", ImGuiDataType_Float
                  , &(rlgl->m_camera.m_renderScale), 0.1f, &s_rs_min, &s_rs_max);
                rlgl->m_camera.m_renderScale = max(rlgl->m_camera.m_renderScale, s_rs_min);

                static float s_cfov_min = 10.0f;
                static float s_cfov_max = 120.0f;
                ImGui::DragScalar("Camera FOV", ImGuiDataType_Float
                  , &(rlgl->cameraFOV), 0.5f, &s_cfov_min, &s_cfov_max);

                static float s_cfp_min = 10.0f;
                static float s_cfp_max = 2000.0f;
                ImGui::DragScalar("Camera Far Plane", ImGuiDataType_Float
                  , &(rlgl->cameraFarPlane), 0.5f, &s_cfp_min, &s_cfp_max);
              }
              ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader("Shadows Settings", treeNodeFlag))
            {
              ImGui::Indent();
              {
                static float s_sf_min = 0.0f;
                static float s_sf_max = 1000.0f;
                ImGui::DragScalar("Main Shadow Far Plane", ImGuiDataType_Float
                  , &(rlgl->mainShadowsFarPlane), 0.5f, &s_sf_min, &s_sf_max);

                static int s_mscc_min = 0;
                static int s_mscc_max = 4;
                ImGui::DragScalar("Main Shadow Cascade Count", ImGuiDataType_S32
                  , &(rlgl->mainShadowscascadeCount), 1.0f, &s_mscc_min, &s_mscc_max);

                //Shadows Resolutions
                static int s_currentMSItem = 2;
                static int s_currentPSItem = 1;
                static std::vector <std::string> s_items;
                static std::vector <const char*> s_itemsPtr;
                static bool s_init = false;
                if (!s_init)
                {
                  for (auto res : k_shadowResEnum)
                  {
                    std::string rstr = std::to_string((int)res);
                    s_items.emplace_back(rstr);
                  }
                  for (auto& resStr : s_items)
                  {
                    s_itemsPtr.emplace_back(resStr.c_str());
                  }
                  s_init = true;
                }

                bool changed = ImGui::Combo("Main Shadow Resolution", &s_currentMSItem
                  , s_itemsPtr.data(), s_itemsPtr.size());
                if (changed)
                {
                  rlgl->mainShadowResolution = k_shadowResEnum[s_currentMSItem];
                }

                changed = ImGui::Combo("Point Shadow Resolution", &s_currentPSItem
                  , s_itemsPtr.data(), s_itemsPtr.size());
                if (changed)
                {
                  rlgl->mainShadowResolution = k_shadowResEnum[s_currentPSItem];
                }
              }
              ImGui::Unindent();
            }
          }
        }
        ImGui::EndChild();
      }
      ImGui::EndGroup();
    }
    ImGui::End();
  }

}