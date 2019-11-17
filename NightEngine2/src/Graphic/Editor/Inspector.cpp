
/*!
  @file Inspector.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Inspector
*/

#include "Graphic/Editor/Inspector.hpp"
#include "Graphic/Editor/MemberSerializerEditor.hpp"
#include "Graphic/Editor/GameObjectHierarchy.hpp"
#include "Graphic/Editor/ConfirmationBox.hpp"
#include "imgui/imgui.h"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/ArchetypeManager.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphic/Window.hpp"

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
  static ConfirmationBox g_confirmBox{ConfirmationBox::BoxType::Popup };
  static std::string     g_confirmBoxDesc{ "" };

  //Component/GameObject Editor Shared States
  static GameObject*     g_curSelectedGameObject = nullptr;
  static bool            g_removeComponent{false};
  static std::string     g_componentToRemove{ "" };

  void Inspector::Update(MemberSerializerEditor& memberSerializer
    , Hierarchy& hierarchy)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer, hierarchy);
    }
    g_confirmBox.Update();
  }

  void Inspector::Draw(bool* show, MemberSerializerEditor& memberSerializer
    , Hierarchy& hierarchy)
  {
    g_curSelectedGameObject = hierarchy.GetSelectedGameObject();

    float width = 310.0f;
    float height = Window::GetHeight() * 0.5f;
    float topMenuHeight = 20.0f;
    ImGui::SetNextWindowPos(ImVec2(Window::GetWidth() - width, topMenuHeight + height), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(width, height - topMenuHeight), ImGuiCond_Appearing);
    
    //********************************************************
    // Window
    //********************************************************
    if (ImGui::Begin("Inspector", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      // Component Inspector
      ImGui::BeginGroup();
      {
        ImGui::BeginChild("GameObject Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          //GameObject Name header
          ImGui::TextColored(g_color_orange_dark, "GameObject: ");
          ImGui::SameLine();
          std::string name{ "" };
          if (g_curSelectedGameObject == nullptr)
          {
            name += "None";
          }
          else
          {
            name += "\"";
            name += g_curSelectedGameObject->GetName();
            name += "\"";
          }
          ImGui::Text(name.c_str());
          ImGui::Separator();

          //GameObject Property, if not null
          if (g_curSelectedGameObject != nullptr)
          {
            ImGui::TextColored(g_color_orange_light
              , "Components Count: ");
            ImGui::SameLine();
            auto count = g_curSelectedGameObject->GetComponentCount();
            ImGui::Text("%d", count + 1);

            //Transform Header
            if (ImGui::CollapsingHeader("Transform"))
            {
              //Traverse MetaType's Member
              auto transform = g_curSelectedGameObject->GetTransform();
              auto metatype = METATYPE_FROM_OBJECT(*(transform));
              auto& members = metatype->GetMembers();

              //Update Angle Checkbox
              static bool updateAngle = false;
              ImGui::Checkbox("Update Angle", &updateAngle);
              ImGui::Separator();

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
                memberSerializer.DrawMemberEditor(member, transform);
                ImGui::NextColumn();
                ImGui::Separator();
              }

              //Update Angle
              if (updateAngle)
              {
                transform->SetEulerAngle(transform->GetEulerAngle());
              }
            }
            ImGui::Separator();

            //All Components
            auto& components = g_curSelectedGameObject->GetAllComponents();
            for (auto& component : components)
            {
              //Traverse MetaType's Member
              auto metatype = component.m_metaType;
              auto& typeName = component.m_metaType->GetName();

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
                  memberSerializer.DrawMemberEditor(member, component.Get<void*>());
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
              && g_curSelectedGameObject != nullptr)
            {
              g_confirmBoxDesc = "remove \"";
              g_confirmBoxDesc += g_componentToRemove;
              g_confirmBoxDesc += "\" component from ";
              g_confirmBoxDesc += g_curSelectedGameObject->GetName();
              g_confirmBox.ShowConfirmationBox( const_cast<char*>(g_confirmBoxDesc.c_str()),
              []()
              {
                Debug::Log << "Remove Component: " << g_componentToRemove.c_str() << '\n';
                g_curSelectedGameObject->RemoveComponent(g_componentToRemove.c_str());
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
              if (ImGui::Button("Cancel", ImVec2(width*0.5f, 30.0f)))
              {
                addComponentMenu = false;
              }
              ImGui::SameLine();
              //Add Button
              if (ImGui::Button("Add", ImVec2(width*0.5f, 30.0f)))
              {
                addComponentMenu = false;
                if (selectedComponent != ""
                  && g_curSelectedGameObject != nullptr)
                {
                  g_curSelectedGameObject->AddComponent(selectedComponent.c_str());
                  selectedComponent = "";
                }
              }

              //Components List
              ImGui::BeginChild("Components List", ImVec2(0, 0)
                ,true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_AlwaysVerticalScrollbar);
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
            else if (ImGui::Button("Add Component", ImVec2(width - 20, 30.0f)))
            {
              addComponentMenu = true;
            }

          }
        }
        ImGui::EndChild();


        //********************************************************
        // Save Columns
        //********************************************************
        ImGui::Separator();
        if (g_curSelectedGameObject != nullptr)
        {
          const int buttonAmount = 3;
          auto size = ImVec2((width - 30) / buttonAmount, 0.0f);
          if (ImGui::Button("Revert", size))
          {
            //TODO: Confirmation box before deserialize value from file
            //Revert to Blueprint
            /*ArchetypeManager::LoadArchetype(g_curSelectedGameObject->GetName()
              , *g_curSelectedGameObject);*/
          }
          ImGui::SameLine();

          if (ImGui::Button("Save Blueprint", size))
          {
            std::string desc{ "save \"" };
            desc += g_curSelectedGameObject->GetName();
            desc += "\" to blueprint: ";
            g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
              , [](void* ptr)
            {
              //Input string
              char* inputName = reinterpret_cast<char*>(ptr);
              std::string name{ inputName };

              ArchetypeManager::SaveToBlueprint(name
                , *g_curSelectedGameObject);
            });
          }
          ImGui::SameLine();

          if (ImGui::Button("Save Archetype", size))
          {
            std::string desc{ "save \"" };
            desc += g_curSelectedGameObject->GetName();
            desc += "\" to archetype: ";
            g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
              , [](void* ptr)
            {
              //Input string
              char* inputName = reinterpret_cast<char*>(ptr);
              std::string name{ inputName };

              ArchetypeManager::SaveToArchetype(name
                , *g_curSelectedGameObject);
            });
          }
        }
      }
      ImGui::EndGroup();
    }
    ImGui::End();
  }

}