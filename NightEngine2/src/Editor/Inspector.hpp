/*!
  @file Inspector.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Inspector
*/

#pragma once
namespace Editor
{
  //Forward declaraction
  class MemberSerializerEditor;
  class Hierarchy;

  class Inspector
  {
  public:
    //! @brief Constructor
    Inspector(void) {}

    //! @brief Update
    void Update(MemberSerializerEditor& memberSerializer
      , Hierarchy& hierarchy);

    //! @brief Draw the Inspector
    void Draw(bool* show, MemberSerializerEditor& memberSerializer
        , Hierarchy& hierarchy);

    //! @brief Get reference to boolean
    bool& GetBool(void) { return m_show; }
  private:
    bool m_show = true;
  };
  
}