
/*!
  @file GameObjectHierarchy.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of GameObjectHierarchy
*/

#include "Editor/GameObjectHierarchy.hpp"
#include "imgui/imgui.h"
#include "Graphic/Opengl/Window.hpp"

#include "Core/EC/GameObject.hpp"

using namespace Core;
using namespace Graphic;
using namespace Core::ECS;

namespace Editor
{
  void Hierarchy::Update(void)
  {
    if (m_show)
    {
      Draw(&m_show);
    }
  }

  void Hierarchy::Draw(bool * open)
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
    //TODO: Loop for each openning scene
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
    {
      //TODO: Some container to hold all selected index maybe set
      int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
      ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2); // Increase spacing to differentiate leaves from expanded contents.

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
          //Node Selection
          auto mapIt = m_selectedIndexSet.find(gameObjectIndex);
          ImGuiTreeNodeFlags node_flags = mapIt != m_selectedIndexSet.end()?//selection_mask & (1 << gameObjectIndex) ? 
            ImGuiTreeNodeFlags_Selected : 0;

          //Node openable flag
          bool hasChild = false;

          //TODO: Traverse child
          if (hasChild)
          {
            node_flags |= ImGuiTreeNodeFlags_OpenOnArrow
              | ImGuiTreeNodeFlags_OpenOnDoubleClick;

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
              | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

            ImGui::TreeNodeEx((void*)(intptr_t)gameObjectIndex
              , node_flags, name);
          }

          //Select Node
          if (ImGui::IsItemClicked())
          {
            node_clicked = gameObjectIndex;
            m_curSelected = it.Get();
          }
        }

        //Next iterator
        it.Next();
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
  }
}