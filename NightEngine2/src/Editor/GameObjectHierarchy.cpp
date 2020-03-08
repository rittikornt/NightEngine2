
/*!
  @file GameObjectHierarchy.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameObjectHierarchy
*/

#include "Editor/GameObjectHierarchy.hpp"
#include "imgui/imgui.h"
#include "Graphics/Opengl/Window.hpp"

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/Scene.hpp"

#include "Core/EC/GameObject.hpp"

using namespace NightEngine;
using namespace Rendering;
using namespace NightEngine::EC;

namespace Editor
{
  void Hierarchy::Update(void)
  {
    if (m_show)
    {
      Draw(&m_show);
    }
  }

  void Hierarchy::Draw(bool* open)
  {
    float height = Window::GetHeight() * 0.5f;
    ImGui::SetNextWindowPos(ImVec2(Window::GetWidth() - 310, 20), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(310, height), ImGuiCond_Appearing);
    if (ImGui::Begin("Hierachy", open
      , ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove))
    {
      //Filter Search
      static ImGuiTextFilter filter;
      filter.Draw("Search");

      // Tree Hierarchy in Child Border
      ImGui::BeginChild("Panel", ImVec2(0, 0), true
        , ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
      {
        DrawHierarchyTree(filter);
      }
      ImGui::EndChild();
    }
    ImGui::End();
  }

  void Hierarchy::DrawHierarchyTree(ImGuiTextFilter& filter)
  {
    static std::vector<bool> closable_groups;
    static bool cur_closable_group = true;

    //Loop for all openning scene
    auto scenes = SceneManager::GetAllScenes();
    for (int i = 0; i < scenes->size(); ++i)
    {
      //Make sure there's enough size
      if (closable_groups.size() < i + 1)
      {
        closable_groups.emplace_back(true);
      }
      cur_closable_group = (closable_groups[i]);

      Scene* scenePtr = (*scenes)[i].Get();
      if ( ImGui::CollapsingHeader(scenePtr->GetSceneName().c_str()
        , &cur_closable_group, ImGuiTreeNodeFlags_DefaultOpen) )
      {
        //TODO: Some container to hold all selected index maybe set
        int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.

        //Traverse all gameobject
        int gameObjectIndex = -1;
        auto& gameObjects = scenePtr->GetAllGameObjects();

        for(int gID = 0; gID < gameObjects.size(); ++gID)
        {
          gameObjectIndex = gameObjects[gID].GetSlotMapID();

          //Draw the Column, if name pass the filter
          auto name = gameObjects[gID]->GetName().c_str();
          if (filter.PassFilter(name))
          {
            //Node Selection
            auto setIt = m_selectedIndexSet.find(gameObjectIndex);
            ImGuiTreeNodeFlags node_flags = setIt != m_selectedIndexSet.end() ?//selection_mask & (1 << gameObjectIndex) ? 
              ImGuiTreeNodeFlags_Selected : 0;

            //Node openable flag
            bool hasChild = false;

            //TODO: Traverse child
            if (hasChild)
            {
              node_flags |= ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_SpanAvailWidth;

              bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)gameObjectIndex
                , node_flags, name);
              if (node_open)
              {
                //TODO: Recursively draw Child Node
                ImGui::Text("Blah blah\nBlah Blah");
                ImGui::Text("Blah blah\nBlah Blah");
                ImGui::Text("Blah blah\nBlah Blah");

                node_flags |= ImGuiTreeNodeFlags_Leaf
                  | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                ImGui::TreeNodeEx((void*)(intptr_t)gameObjectIndex
                  , node_flags, "Selectable Leaf %d", gameObjectIndex);

                ImGui::TreePop();
              }
            }
            //No child
            else
            {
              node_flags |= ImGuiTreeNodeFlags_Leaf
                | ImGuiTreeNodeFlags_NoTreePushOnOpen
                | ImGuiTreeNodeFlags_SpanAvailWidth; // ImGuiTreeNodeFlags_Bullet

              ImGui::TreeNodeEx((void*)(intptr_t)gameObjectIndex
                , node_flags, name);
            }

            //Select Node
            if (ImGui::IsItemClicked())
            {
              node_clicked = gameObjectIndex;
              m_curSelected = gameObjects[gID].Get();
            }
          }
        }

        //Click on some Node
        if (node_clicked != -1)
        {
          if (ImGui::GetIO().KeyCtrl)
          {
            m_selectedIndexSet.insert({ node_clicked });
          }
          else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
          {
            m_selectedIndexSet.clear();
            m_selectedIndexSet.insert({ node_clicked });
          }
        }
        ImGui::PopStyleVar();
      }

      //Save flags if the close button is clicked
      closable_groups[i] = cur_closable_group;
    }

    //Check for close button click flag
    for (int i = 0; i < closable_groups.size(); ++i)
    {
      if (closable_groups[i] == false)
      {
        Debug::Log << "Close Scene: " << (*scenes)[i].Get()->GetSceneName() << '\n';
        m_curSelected = nullptr;
        SceneManager::CloseScene((*scenes)[i]);

        //Reopen all the closed headers
        for (int i = 0; i < closable_groups.size(); ++i)
        {
          closable_groups[i] = true;
        }

        break;
      }
    }
  }
}