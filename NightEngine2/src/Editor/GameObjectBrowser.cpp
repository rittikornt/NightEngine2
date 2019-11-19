/*!
  @file GameObjectBrowser.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameObjectBrowser
*/
#include "Editor/GameObjectBrowser.hpp"
#include "Editor/MemberSerializerEditor.hpp"
#include "Editor/ConfirmationBox.hpp"

#include "imgui/imgui.h"

#include "Core/EC/GameObject.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/EC/ArchetypeManager.hpp"
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
  static ConfirmationBox g_confirmBox{ ConfirmationBox::BoxType::Popup };
  static std::string     g_confirmBoxDesc{ "" };

  //Component/GameObject Editor Shared States
  static GameObject*     g_curSelectedGameObject = nullptr;
  static int             g_selectedIndex = 0;

  static bool            g_removeComponent{ false };
  static std::string     g_componentToRemove{ "" };

  void GameObjectBrowser::Update(MemberSerializerEditor& memberSerializer)
  {
    if (m_show)
    {
      Draw(&m_show, memberSerializer);
    }
    g_confirmBox.Update();
  }

  void GameObjectBrowser::Draw(bool* show, MemberSerializerEditor& memberSerializer)
  {
    static bool show_leftPanel = true;

    float width = 500;
    float leftPanelWidth = 0;
    ImGui::SetNextWindowPos(ImVec2(185, 20), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(width, 500), ImGuiCond_Appearing);
    
    //Begin Window
    if (ImGui::Begin("GameObject Browser", show
      , ImGuiWindowFlags_NoSavedSettings))
    {
      width = ImGui::GetWindowWidth();
      // Left Panel
      if (show_leftPanel)
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
            ImGui::Columns(2, "GameObject Column", true);
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("NAME"); ImGui::NextColumn();
            ImGui::Separator();

            //Traverse all gameobject
            int gameObjectIndex = -1;
            auto& gameObjectContainer = Factory::GetTypeContainer<GameObject>();
            auto it = gameObjectContainer.GetIterator();
            while (!it.IsEnd())
            {
              gameObjectIndex = it.m_index;

              //Draw the Column, if name pass the filter
              auto name = it.Get()->GetName().c_str();
              if (filter.PassFilter(name))
              {
                //ID Column 
                char label[8];
                sprintf(label, "%d", gameObjectIndex);
                if (ImGui::Selectable(label, g_selectedIndex == gameObjectIndex))
                {
                  g_selectedIndex = gameObjectIndex;
                  g_curSelectedGameObject = it.Get();
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
        ImGui::BeginChild("GameObject Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
        {
          ImGui::Columns(2, "Right Header");
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
          ImGui::NextColumn();

          //Checkbox for enable left panel
          ImGui::Checkbox("GameObject List", &show_leftPanel);
          ImGui::Separator();
          ImGui::Columns(1, "Right Information");

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
              g_confirmBox.ShowConfirmationBox(const_cast<char*>(g_confirmBoxDesc.c_str()),
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
              if (ImGui::Button("Cancel", ImVec2((width- leftPanelWidth)*0.5f, 30.0f)))
              {
                addComponentMenu = false;
              }
              ImGui::SameLine();
              //Add Button
              if (ImGui::Button("Add", ImVec2((width - leftPanelWidth)*0.5f, 30.0f)))
              {
                addComponentMenu = false;
                if (selectedComponent != ""
                  && g_curSelectedGameObject != nullptr)
                {
                  g_curSelectedGameObject->AddComponent(selectedComponent.c_str());
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
            else if (ImGui::Button("Add Component", ImVec2((width - leftPanelWidth - 20), 30.0f)))
            {
              addComponentMenu = true;
            }
          }
        }
        ImGui::EndChild();

        if (g_curSelectedGameObject != nullptr)
        {
          const int buttonAmount = 4;
          auto size = ImVec2((width - leftPanelWidth - 70) / buttonAmount, 0.0f);
          //Lower Right Buttons
          if (ImGui::Button("Revert", size))
          {
            std::string desc{ "revert \"" };
            desc += g_curSelectedGameObject->GetName();
            desc += "\" archetype to original";
            g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
              , []()
            {
              //TODO: revert from the referenced Archetype on the sceneNode
              //ArchetypeManager::LoadArchetype(,);
            });
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
          if (ImGui::Button("Load Blueprint", size))
          {
            std::string desc{ "load \"" };
            desc += g_curSelectedGameObject->GetName();
            desc += "\" blueprint to gameobject: ";
            g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
              , [](void* ptr)
            {
              //Input string
              char* inputName = reinterpret_cast<char*>(ptr);
              std::string name{ inputName };

              ArchetypeManager::LoadBlueprint(name
                , *g_curSelectedGameObject);
            });
          }
          ImGui::SameLine();
          if (ImGui::Button("Save to Archetype", size))
          {
            std::string desc{ "save \"" };
            desc += g_curSelectedGameObject->GetName();
            desc += "\" to archetype";
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

  void GameObjectBrowser::DrawHeaderButton(void)
  {
    //********************************************
    //  Header Buttons
    //********************************************
    ImGui::Columns(1, "GameObject Buttons", true);
    const float buttonAmount = g_curSelectedGameObject != nullptr ? 3.0f : 1.0f;
    auto size = ImVec2((ImGui::GetWindowContentRegionWidth() - 15) / buttonAmount, 0.0f);
    //Add GameObject Button
    if (ImGui::Button("Add"
      , size))
    {
      std::string desc{ "add gameobject to scene: " };
      g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
        , [](void* ptr)
      {
        //Input string
        char* inputName = reinterpret_cast<char*>(ptr);
        std::string name{ inputName };

        GameObject::Create(name.c_str(), 1);
        g_curSelectedGameObject = nullptr;
        g_selectedIndex = 0;
      });
    }
    if (g_curSelectedGameObject != nullptr)
    {
      ImGui::SameLine();
      if (ImGui::Button("Rename", size)
        && g_curSelectedGameObject != nullptr)
      {
        std::string desc{ "rename gameobject: " };
        g_confirmBox.ShowConfirmationBoxWithInput(const_cast<char*>(desc.c_str())
          , [](void* ptr)
        {
          //Input string
          char* inputName = reinterpret_cast<char*>(ptr);
          std::string name{ inputName };

          g_curSelectedGameObject->SetName(name);
        });
      }
      //Remove Archetype Button
      ImGui::SameLine();
      if (ImGui::Button("Remove", size))
      {
        std::string desc{ "remove \"" };
        desc += g_curSelectedGameObject->GetName();
        desc += "\" from scene";
        g_confirmBox.ShowConfirmationBox(const_cast<char*>(desc.c_str())
          , []()
        {
          g_curSelectedGameObject->Destroy();
          g_curSelectedGameObject = nullptr;
          g_selectedIndex = 0;
        });
      }
    }
  }

  
}