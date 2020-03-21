/*!
  @file MemberSerializerEditor.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MemberSerializerEditor
*/
#pragma once
#include <unordered_map>
#include "Core/Container/MurmurHash2.hpp"
#include <string>

namespace NightEngine
{
  namespace Reflection
  {
    class Variable;
    class Member;
  }
}

namespace Editor
{
  //! @brief Class for serialize member to editor
  class MemberSerializerEditor
  {
  public:
    //! @brief Constructor
    MemberSerializerEditor(void);

    //! @brief Draw the editor for member
    bool DrawMemberEditor(NightEngine::Reflection::Member& member
      , void* dataObject, const std::string& nameMingle = "");
  private:
    struct StringHash 
		{
				std::size_t operator()(const std::string& key) const 
				{
					return NightEngine::Container::ConvertToHash(key.c_str(), key.size());
				}
		};
    using EditorFunc = void(*)(NightEngine::Reflection::Variable&, const char*);
    using TypeEditorFuncMap = std::unordered_map<std::string, EditorFunc>;
    TypeEditorFuncMap m_typeEditorMap;
  };
}