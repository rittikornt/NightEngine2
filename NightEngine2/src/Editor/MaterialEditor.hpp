/*!
  @file MaterialEditor.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MaterialEditor
*/
#pragma once
namespace Editor
{
  //Forward declaraction
  class MemberSerializerEditor;

  class MaterialEditor
  {
  public:
    MaterialEditor(void) {}

    //! @brief Update
    void Update(MemberSerializerEditor& memberSerializer);

    //! @brief Draw the Editor
    void Draw(bool* show, MemberSerializerEditor& memberSerializer);

    //! @brief Draw Header Button
    void DrawHeaderButton(void);

    //! @brief Reference to the show boolean
    bool& GetBool(void) { return m_show; }
  private:
    bool m_show = false;
  };
  
}