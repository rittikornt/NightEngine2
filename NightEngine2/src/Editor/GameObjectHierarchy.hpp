/*!
  @file GameObjectHierarchy.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GameObjectHierarchy
*/
#pragma once
#include <unordered_set>

//Forward declaration
struct ImGuiTextFilter;

namespace NightEngine
{
  namespace EC
  {
    class GameObject;
  }
}

namespace Editor
{
  class Hierarchy
  {
  public:
    //! @brief Constructor
    Hierarchy(void){}

    //! @brief Update
    void Update(void);

    //! @brief Draw the Hierarchy
    void Draw(bool* open);

    //! @brief Draw the Hierarchy Tree
    void DrawHierarchyTree(ImGuiTextFilter& filter);

    //! @brief Get reference to boolean
    inline bool& GetBool(void) { return m_show; }

    //! @brief Get selected gameobject
    inline NightEngine::EC::GameObject* GetSelectedGameObject(void) { return m_curSelected; }
  private:
    NightEngine::EC::GameObject* m_curSelected;
    std::unordered_set<int> m_selectedIndexSet;
    bool m_show = true;
  };
  
}