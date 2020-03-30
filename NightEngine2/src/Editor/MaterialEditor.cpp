/*!
  @file MaterialEditor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MaterialEditor
*/
#include "Editor/MaterialEditor.hpp"
#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/ConfirmationBox.hpp"

#include "imgui/imgui.h"

#include "Core/EC/Handle.hpp"
#include "Core/Container/Slotmap.hpp"
#include "Core/EC/ArchetypeManager.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphics/Opengl/Window.hpp"
#include "Graphics/Opengl/Material.hpp"

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/Scene.hpp"

using namespace NightEngine;
using namespace NightEngine::EC;
using namespace Rendering;
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

  static Slotmap<Material>::Iterator  g_curSelectedMaterial;
  static int                          g_selectedIndex = 0;

  static bool            g_removeComponent{ false };
  static std::string     g_componentToRemove{ "" };

  void MaterialEditor::Update(MemberSerializerEditor& memberSerializer)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer);
    }
    g_confirmBox.Update();
  }

  void MaterialEditor::Draw(bool* show, MemberSerializerEditor& memberSerializer)
  {
    static bool s_showLeftPanel = true;

    float width = 500;
    float leftPanelWidth = 0;
    ImGui::SetNextWindowPos(ImVec2(185, 20), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(width, 500), ImGuiCond_Appearing);
    
    //Begin Window
    if (ImGui::Begin("Material Editor", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      // Left Panel
      if (s_showLeftPanel)
      {
        //Columns between left and Right Panels
        ImGui::Columns(2, "LeftRightPanels", true);

        ImGui::BeginChild("Left Panel", ImVec2(leftPanelWidth, 0), true
        , ImGuiWindowFlags_AlwaysAutoResize);
        {
          static ImGuiTextFilter filter;
          filter.Draw("Search", 110.0f);

          //Header buttons
          ImGui::Separator();
          DrawHeaderButton();

          leftPanelWidth = ImGui::GetWindowContentRegionWidth();

          //Bottom Left
          ImGui::BeginChild("Bottom Left Panel", ImVec2(0, 0), true
            , ImGuiWindowFlags_AlwaysVerticalScrollbar
            | ImGuiWindowFlags_AlwaysHorizontalScrollbar
            | ImGuiWindowFlags_AlwaysAutoResize);
          {
            //Column Header
            ImGui::Columns(2, "Material Column", true);
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("NAME"); ImGui::NextColumn();
            ImGui::Separator();

            //Traverse all material
            int matIndex = -1;
            auto& materialContainer = Factory::GetTypeContainer<Material>();
            auto it = materialContainer.GetIterator();
            while (!it.IsEnd())
            {
              matIndex = it.m_index;

              //Draw the Column, if name pass the filter
              auto name = it.Get()->GetName().c_str();
              if (filter.PassFilter(name))
              {
                //ID Column 
                char label[8];
                sprintf(label, "%d", matIndex);
                if (ImGui::Selectable(label, g_selectedIndex == matIndex))
                {
                  g_selectedIndex = matIndex;
                  g_curSelectedMaterial = it;
                }

                //Name Column
                ImGui::NextColumn();
                ImGui::Text(name); ImGui::NextColumn();
              }

              //Next iterator
              it.Next();
            }

            //EndChild
            ImGui::EndChild();
          }
        }
        ImGui::EndChild();
        ImGui::SameLine();

        //Columns between left and Right Panels
        ImGui::Columns(2, "LeftRightPanels", true);
        ImGui::NextColumn();
      }

      // Right Panel
      ImGui::BeginGroup();
      {
        ImGui::BeginChild("Material Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          ImGui::Columns(2, "Right Header");
          //Material Name header
          ImGui::TextColored(g_color_orange_dark, "Material: ");
          ImGui::SameLine();
          std::string name{ "" };
          if (g_curSelectedMaterial.IsEnd())
          {
            name += "None";
          }
          else
          {
            name += "\"";
            name += g_curSelectedMaterial.Get()->GetName();
            name += "\"";
          }
          ImGui::Text(name.c_str());
          ImGui::NextColumn();

          //Checkbox for enable left panel
          ImGui::Checkbox("Material List", &s_showLeftPanel);
          ImGui::Separator();
          ImGui::Columns(1, "Right Information");

          //Material Property, if not null
          if (!(g_curSelectedMaterial.IsEnd()))
          {
            //Material Editor
            auto material = g_curSelectedMaterial.Get();
            memberSerializer.DrawMetaTypeEditor(METATYPE_FROM_OBJECT(*material)
              , material, std::to_string(0));
          }
        }
        ImGui::EndChild();
      }
      ImGui::EndGroup();
    }
    ImGui::End();
  }

  void MaterialEditor::DrawHeaderButton(void)
  {
    //********************************************
    //  Header Buttons
    //********************************************
    ImGui::Columns(1, "Material Buttons", true);
    const float buttonAmount = !(g_curSelectedMaterial.IsEnd()) ? 3.0f : 1.0f;
    auto size = ImVec2((ImGui::GetWindowContentRegionWidth() - 15) / buttonAmount, 0.0f);
    
    //Add Material Button
    if (ImGui::Button("Add"
      , size))
    {
      std::string desc{ "Create new material file: " };
      g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
        , [](void* ptr)
      {
        //Input string
        char* inputName = reinterpret_cast<char*>(ptr);
        std::string name{ inputName };

        Handle<Material> handle = Factory::Create<Material>("Material");
        handle.Get()->SetName(name);

        g_curSelectedMaterial = Slotmap<Material>::Iterator{};
        g_selectedIndex = 0;
      });
    }

    if (!(g_curSelectedMaterial.IsEnd()))
    {
      ImGui::SameLine();
      if (ImGui::Button("Rename", size))
      {
        std::string desc{ "rename gameobject: " };
        g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
          , [](void* ptr)
        {
          //Input string
          char* inputName = reinterpret_cast<char*>(ptr);
          std::string name{ inputName };

          g_curSelectedMaterial.Get()->SetName(name);
        });
      }

      //Remove Material
      ImGui::SameLine();
      if (ImGui::Button("Remove", size))
      {
        std::string desc{ "remove \"" };
        desc += g_curSelectedMaterial.Get()->GetName();
        desc += "\" from scene";
        g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
          , []()
        {
            auto id = g_curSelectedMaterial.ToSlotmapID();
            g_curSelectedMaterial.m_slotmap->Destroy(id);

            g_curSelectedMaterial = Slotmap<Material>::Iterator{};
            g_selectedIndex = 0;
        });
      }
    }
  }

  
}