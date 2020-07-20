/*!
  @file RenderSettingEditor.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderSettingEditor
*/

#pragma once
namespace Editor
{
  //Forward declaraction
  class MemberSerializerEditor;
  class Hierarchy;

  class RenderSettingEditor
  {
  public:
    //! @brief Constructor
    RenderSettingEditor(void) {}

    //! @brief Update
    void Update(MemberSerializerEditor& memberSerializer);

    //! @brief Draw the RenderSettingEditor
    void Draw(bool* show, MemberSerializerEditor& memberSerializer);

    //! @brief Get reference to boolean
    bool& GetBool(void) { return m_show; }
  private:
    bool m_show = true;
  };
  
}