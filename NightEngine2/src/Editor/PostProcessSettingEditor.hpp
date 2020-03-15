/*!
  @file PostProcessSettingEditor.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessSettingEditor
*/

#pragma once
namespace Editor
{
  //Forward declaraction
  class MemberSerializerEditor;
  class Hierarchy;

  class PostProcessSettingEditor
  {
  public:
    //! @brief Constructor
    PostProcessSettingEditor(void) {}

    //! @brief Update
    void Update(MemberSerializerEditor& memberSerializer);

    //! @brief Draw the PostProcessSettingEditor
    void Draw(bool* show, MemberSerializerEditor& memberSerializer);

    //! @brief Get reference to boolean
    bool& GetBool(void) { return m_show; }
  private:
    bool m_show = true;
  };
  
}