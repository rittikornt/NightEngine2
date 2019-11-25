/*!
  @file ArchetypeBrowser.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ArchetypeBrowser
*/
#include "Editor/ArchetypeBrowser.hpp"
#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/ConfirmationBox.hpp"

#include "imgui/imgui.h"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/Archetype.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphic/Opengl/Window.hpp"

#include "Core/EC/ArchetypeManager.hpp"

using namespace Core;
using namespace Core::ECS;
using namespace Graphic;
using namespace Reflection;

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);
  static ImVec4 g_color_orange_dark = ImVec4(0.96, 0.68f, 0.05f, 1.0f);
  static ImVec4 g_color_orange_light = ImVec4(0.96, 0.86f, 0.05f, 1.0f);
  static ImVec4 g_color_green = ImVec4(0.165f, 0.86f, 0.33f, 1.0f);

  //Confirmation Box
  static ConfirmationBox g_confirmBox{ ConfirmationBox::BoxType::Popup };
  static std::string     g_confirmBoxDesc{ "" };

  //Component/GameObject Editor Shared States
  static Archetype*      g_curSelectedArchetype = nullptr;
  static int             g_selectedIndex = 0;
  static bool            g_removeComponent{ false };
  static std::string     g_componentToRemove{ "" };

  void ArchetypeBrowser::Update(MemberSerializerEditor& memberSerializer)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer);
    }
    g_confirmBox.Update();
  }

  void ArchetypeBrowser::Draw(bool* show, MemberSerializerEditor& memberSerializer)
  {
    float width = 500;
    float leftPanelWidth = 0;
    ImGui::SetNextWindowPos(ImVec2(185, 20), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(width, 500), ImGuiCond_Appearing);
    
    //Begin Window
    if (ImGui::Begin("Archetype Browser", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      // Left Panel
      if (m_showLeftPanel)
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
            ImGui::Columns(2, "Archetype Column", true);
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("NAME"); ImGui::NextColumn();
            ImGui::Separator();

            //Traverse ArchetypeMap
            auto& map = ArchetypeManager::GetArchetypeMap();
            int archetypeIndex = -1;
            for (auto& pair : map)
            {
              //Next Archetype
              ++archetypeIndex;

              auto name = pair.first.c_str();
              if (filter.PassFilter(name))
              {
                //ID Column 
                char label[8];
                sprintf(label, "%d", archetypeIndex);
                if (ImGui::Selectable(label, g_selectedIndex == archetypeIndex))
                {
                  g_selectedIndex = archetypeIndex;
                  g_curSelectedArchetype = &pair.second;
                }

                //Name Column
                ImGui::NextColumn();
                ImGui::Text(name); ImGui::NextColumn();
              }
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
        ImGui::BeginChild("Archetype Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          ImGui::Columns(2, "Right Header");
          //Archetype Name header
          ImGui::TextColored(g_color_orange_dark, "Archetype: ");
          ImGui::SameLine();
          std::string name{ "" };
          if (g_curSelectedArchetype == nullptr)
          {
            name += "None";
          }
          else
          {
            name += "\"";
            name += g_curSelectedArchetype->m_name;
            name += "\"";
          }
          ImGui::Text(name.c_str());
          ImGui::NextColumn();

          //Checkbox for enable left panel
          ImGui::Checkbox("Archetype List", &m_showLeftPanel);
          ImGui::Separator();
          ImGui::Columns(1, "Right Information");

          //GameObject Property, if not null
          if (g_curSelectedArchetype != nullptr)
          {
            ImGui::TextColored(g_color_orange_light
              , "Components Count: ");
            ImGui::SameLine();
            auto count = g_curSelectedArchetype->m_componentTypes.size();
            ImGui::Text("%d", count + 1);

            //Transform Header
            if (ImGui::CollapsingHeader("Transform"))
            {
              //Traverse MetaType's Member
              auto metatype = METATYPE_FROM_STRING("Transform");
              auto& members = metatype->GetMembers();

              //Column header
              ImGui::Columns(3, "Transform Property");
              ImGui::TextColored(g_color_green, "TYPE"); ImGui::NextColumn();
              ImGui::TextColored(g_color_green, "NAME"); ImGui::NextColumn();
              ImGui::TextColored(g_color_green, "VALUE"); ImGui::NextColumn();
              ImGui::Separator();

              //Members
              for (auto& member : members)
              {
                //Type
                ImGui::TextColored(g_color_blue
                  , member.GetMetaType()->GetName().c_str());
                ImGui::NextColumn();

                //Member Name
                ImGui::Text(member.GetName().c_str());
                ImGui::NextColumn();

                //Value Editor
                //memberSerializer.DrawMemberEditor(member, transform);
                ImGui::NextColumn();
                ImGui::Separator();
              }
            }
            ImGui::Separator();

            //All Components
            auto& components = g_curSelectedArchetype->m_componentTypes;
            for (auto& component : components)
            {
              //Traverse MetaType's Member
              auto metatype = METATYPE_FROM_STRING(component);
              auto& typeName = metatype->GetName();

              ImGui::Columns(1, "Component Header");
              bool showComponent = true;
              if (ImGui::CollapsingHeader(typeName.c_str(), &showComponent))
              {
                //Column header
                ImGui::Columns(3, "Component Property");
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
                  //memberSerializer.DrawMemberEditor(member, component.Get<void*>());
                  ImGui::NextColumn();
                  ImGui::Separator();
                }
              }
              ImGui::Separator();

              //If press the 'x' button
              if (!showComponent)
              {
                g_componentToRemove = typeName;
                g_removeComponent = true;
              }
            }

            //Remove Component
            if (g_removeComponent
              && g_componentToRemove != ""
              && g_curSelectedArchetype != nullptr)
            {
              g_confirmBoxDesc = "remove \"";
              g_confirmBoxDesc += g_componentToRemove;
              g_confirmBoxDesc += "\" component from ";
              g_confirmBoxDesc += g_curSelectedArchetype->m_name;
              g_confirmBox.ShowConfirmationBox(const_cast<char*>(g_confirmBoxDesc.c_str()),
                []()
              {
                Debug::Log << "Remove Component: " << g_componentToRemove.c_str() << '\n';
                g_curSelectedArchetype->RemoveComponent(g_componentToRemove);
                g_componentToRemove = "";
              });

              g_removeComponent = false;
            }

            //Clear Column
            ImGui::Columns(1);

            //Add Component
            static bool addComponentMenu = false;
            static std::string selectedComponent = "";
            if (addComponentMenu)
            {
              //Cancel Button
              if (ImGui::Button("Cancel", ImVec2((width- leftPanelWidth - 70)*0.5f, 30.0f)))
              {
                addComponentMenu = false;
              }
              ImGui::SameLine();
              //Add Button
              if (ImGui::Button("Add", ImVec2((width - leftPanelWidth - 70)*0.5f, 30.0f)))
              {
                addComponentMenu = false;
                if (selectedComponent != ""
                  && g_curSelectedArchetype != nullptr)
                {
                  g_curSelectedArchetype->AddComponent(selectedComponent);
                }
              }

              //Components List
              ImGui::BeginChild("Components List", ImVec2(0, 0)
                , true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
              {
                static auto& map = MetaManager::GetMetaMap();
                static auto componentMetaType = METATYPE_FROM_STRING("ComponentLogic");

                for (auto& pair : map)
                {
                  //Type derived from ComponentLogic
                  if (pair.second->IsDerivedFrom(componentMetaType))
                  {
                    char label[128];
                    sprintf(label, pair.first.c_str());

                    //Selectable
                    bool selected = pair.first == selectedComponent;
                    if (ImGui::Selectable(label, selected))
                    {
                      selectedComponent = pair.first;
                      Debug::Log << "Select: " << selectedComponent.c_str() << '\n';
                    }
                  }
                }
              }
              ImGui::EndChild();
            }
            //Add Component Button
            else if (ImGui::Button("Add Component", ImVec2((width - leftPanelWidth - 70), 30.0f)))
            {
              addComponentMenu = true;
            }
          }
        }
        ImGui::EndChild();

        //Bottom Right Buttons
        ImGui::Separator();
        if (g_curSelectedArchetype != nullptr)
        {
          const int buttonAmount = 2;
          auto size = ImVec2((width - leftPanelWidth- 70) / buttonAmount, 0.0f);
          //Revert Archetype Button
          if (ImGui::Button("Revert", size))
          {
            std::string desc{ "revert \"" };
            desc += g_curSelectedArchetype->m_name;
            desc += "\" archetype to original";
            g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
              , []()
            {
              ArchetypeManager::RefreshArchetypeList();
              g_curSelectedArchetype = nullptr;
              g_selectedIndex = 0;
            });
          }
          ImGui::SameLine();
          //Save Archetype Button
          if (ImGui::Button("Save Archetype", size))
          {
            std::string desc{ "save \"" };
            desc += g_curSelectedArchetype->m_name;
            desc += "\" to archetype";
            g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
              , []()
            {
              ArchetypeManager::SaveToArchetype(g_curSelectedArchetype->m_name
                , *g_curSelectedArchetype);
            });
          }
        }
      }

      ImGui::EndGroup();
    }
    ImGui::End();
  }

  void ArchetypeBrowser::DrawHeaderButton(void)
  {
    //********************************************
    //  Header Buttons
    //********************************************
    ImGui::Columns(1, "Archetype Buttons", true);
    const float buttonAmount = g_curSelectedArchetype != nullptr ? 4.0f : 1.0f;
    auto size = ImVec2((ImGui::GetWindowContentRegionWidth() - 15)/ buttonAmount, 0.0f);
    //Add Archetype Button
    if (ImGui::Button("Add"
      , size))
    {
      std::string desc{ "add archetype: " };
      g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
        , [](void* ptr)
      {
        //Input string
        char* inputName = reinterpret_cast<char*>(ptr);
        std::string name{ inputName };

        Archetype newArchetype;
        ArchetypeManager::SaveToArchetype(name, newArchetype);
        g_curSelectedArchetype = nullptr;
        g_selectedIndex = 0;
      });
    }
    if (g_curSelectedArchetype != nullptr)
    {
      ImGui::SameLine();
      if (ImGui::Button("Instantiate"
        , size))
      {
        std::string desc{ "instantiate archetype \"" };
        desc += g_curSelectedArchetype->m_name;
        desc += "\" into the scene";
        g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
          , [](void* ptr)
        {
          //Input string
          char* inputName = reinterpret_cast<char*>(ptr);
          std::string name{ inputName };

          auto handle = GameObject::Create(name.c_str()
            , g_curSelectedArchetype->m_componentTypes.size());
          ArchetypeManager::LoadArchetype(g_curSelectedArchetype->m_name
            , *handle);

          g_curSelectedArchetype = nullptr;
          g_selectedIndex = 0;
        });
      }
      ImGui::SameLine();
      if (ImGui::Button("Rename", size)
        && g_curSelectedArchetype != nullptr)
      {
        std::string desc{ "rename archetype: " };
        g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
          , [](void* ptr)
        {
          //Input string
          char* inputName = reinterpret_cast<char*>(ptr);
          std::string name{ inputName };

          ArchetypeManager::RenameArchetype(g_curSelectedArchetype->m_name
            , name);
          g_curSelectedArchetype = nullptr;
          g_selectedIndex = 0;
        });
      }
      //Remove Archetype Button
      ImGui::SameLine();
      if (ImGui::Button("Remove", size))
      {
        std::string desc{ "remove \"" };
        desc += g_curSelectedArchetype->m_name;
        desc += "\" from archetype";
        g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
          , []()
        {
          ArchetypeManager::RemoveFromArchetype(g_curSelectedArchetype->m_name);
          g_curSelectedArchetype = nullptr;
          g_selectedIndex = 0;
        });
      }
    }
  }

  
}