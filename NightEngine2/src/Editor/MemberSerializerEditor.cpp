/*!
  @file MemberSerializerEditor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MemberSerializerEditor
*/

#include "Editor/MemberSerializerEditor.hpp"
#include "imgui/imgui.h"

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Serialization/FileSystem.hpp"
#include "Core/Serialization/ResourceManager.hpp"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/SceneManager.hpp"

//#include "Graphics/Opengl/Window.hpp"

#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Graphics/Color.hpp"
#include <unordered_map>

#include "Core/Utility/Utility.hpp"
#include "Core/Utility/Profiling.hpp"

using namespace NightEngine::EC;
using namespace NightEngine;
using namespace NightEngine::Rendering;
using namespace NightEngine::Rendering::Opengl;

namespace NightEngine
{
  namespace IMGUI
  {
    bool DrawVec4Property(IMGUIEditorData data, const char* text, float val[4])
    {
      switch (data.type)
      {
      case NightEngine::IMGUI::IMGUIEditorType::DRAGSCALAR:
        return ImGui::DragFloat4(text, val, data.speed, data.min, data.max);
      case NightEngine::IMGUI::IMGUIEditorType::SLIDER:
        return ImGui::SliderFloat4(text, val, data.min, data.max);
      case IMGUIEditorType::COLOR4:
        return ImGui::ColorEdit4(text, val, ImGuiColorEditFlags_HDR);
      case NightEngine::IMGUI::IMGUIEditorType::INPUTSCALAR:
        return ImGui::InputFloat4(text, val, 3);
      default:
        return ImGui::InputFloat4(text, val, 3);
      }

      return false;
    }

    bool DrawFloatProperty(IMGUIEditorData data, const char* text, float* val)
    {
      switch (data.type)
      {
      case NightEngine::IMGUI::IMGUIEditorType::DRAGSCALAR:
        return ImGui::DragScalar(text, ImGuiDataType_Float, val, data.speed, &data.min, &data.max);
      case NightEngine::IMGUI::IMGUIEditorType::SLIDER:
        return ImGui::SliderFloat(text, val, data.min, data.max);
      case NightEngine::IMGUI::IMGUIEditorType::INPUTSCALAR:
        return ImGui::InputScalar(text, ImGuiDataType_Float, val);
      default:
        return ImGui::InputScalar(text, ImGuiDataType_Float, val);
      }

      return false;
    }

    bool DrawIntProperty(IMGUIEditorData data, const char* text, int* val)
    {
      bool bval = (bool)*val;
      bool changed = false;

      switch (data.type)
      {
      case NightEngine::IMGUI::IMGUIEditorType::CHECKBOX:
        changed = ImGui::Checkbox(text, &bval);
        *val = bval? 1.0f: 0.0f;
        return changed;
      case NightEngine::IMGUI::IMGUIEditorType::DRAGSCALAR:
        return ImGui::DragScalar(text, ImGuiDataType_S32, val, data.speed, &data.min, &data.max);
      case NightEngine::IMGUI::IMGUIEditorType::SLIDER:
        return ImGui::SliderInt(text, val, data.min, data.max);
      case NightEngine::IMGUI::IMGUIEditorType::INPUTSCALAR:
        return ImGui::InputScalar(text, ImGuiDataType_S32, val);
      default:
        return ImGui::InputScalar(text, ImGuiDataType_S32, val);
      }

      return false;
    }
  }
}

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);
  static bool g_dirty = false;

  void MemberSerializerEditor::NewFrame(void)
  {
    g_dirty = true;
  }

  static bool DrawTextureComboBox(const char* text, Handle<NightEngine::Rendering::Opengl::Texture>* handle, Texture::Format channel)
  {
    const char* k_none = "None";
    static int s_currentItem = 0;
    static std::vector <std::string> s_items;
    static std::unordered_map <std::string, int> s_itemsMap;
    static std::vector <const char*> s_itemsPtr;
    static std::string  s_texFilePath;

    //Find all Texture name list only once per frame
    if (g_dirty)
    {
      //This is the slow part
      s_items.clear();
      s_items.emplace_back(k_none);

      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Assets, s_items
        , FileSystem::FileFilter::FullPath, ".png", false, true, false);
      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Assets, s_items
        , FileSystem::FileFilter::FullPath, ".jpg", false, true, false);
      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Assets, s_items
        , FileSystem::FileFilter::FullPath, ".tga", false, true, false);

      s_itemsMap.clear();
      for (int i = 0; i < s_items.size(); ++i)
      {
        s_itemsMap.insert({ s_items[i] , i });
      }
      g_dirty = false;
    }

    s_currentItem = 0;
    bool validTex = handle->IsValid();
    s_texFilePath = validTex ? handle->Get()->GetFilePath() : "";

    //TODO: use unordered_set instead to avoid all these string comparison
    //Options input for ImGUI
    s_itemsPtr.clear();
    for (int i = 0; i < s_items.size(); ++i)
    {
      s_itemsPtr.emplace_back(s_items[i].c_str());
    }

    auto it = s_itemsMap.find(s_texFilePath);
    if (validTex && it != s_itemsMap.end())
    {
      s_currentItem = it->second;
    }

    //Combo Box Imgui
    bool changed = ImGui::Combo(text, &s_currentItem, s_itemsPtr.data(), s_itemsPtr.size());
    if (changed)
    {
      if (s_items[s_currentItem] != k_none)
      {
        auto newHandle = Texture::LoadTextureHandle(s_items[s_currentItem]
          , channel);
        if (newHandle.IsValid())
        {
          *(handle) = newHandle;
        }
      }
      else
      {
        //Selecting None
        handle->m_handle.Nullify();
      }
    }
    return changed;
  }

  static bool DrawMaterialComboBox(const char* text, Handle<NightEngine::Rendering::Opengl::Material>* handle)
  {
    static std::vector <std::string> s_items;
    static std::vector <const char*> s_itemsPtr;
    static std::vector <SlotmapID> s_itemsSlotMapID;

    //FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Materials, s_items
    //  , FileSystem::FileFilter::FileName, ".mat", false, true, true);

    //Traverse all material
    s_items.clear();
    s_itemsSlotMapID.clear();

    s_items.emplace_back("<None>");
    s_itemsSlotMapID.emplace_back(SlotmapID{});

    int matIndex = -1;  //actually index into slotmap's array
    int traverseIndex = 0; //traversal index in the loop
    int currentIndex = 0; //chosen traversal index

    auto& materialContainer = Factory::GetTypeContainer<Material>();
    auto it = materialContainer.GetIterator();
    while (!it.IsEnd())
    {
      matIndex = it.m_index;
      s_items.emplace_back(it.Get()->GetName());
      s_itemsSlotMapID.emplace_back(it.ToSlotmapID());

      if (handle->m_handle.m_slotmapID.m_index == matIndex)
      {
        currentIndex = traverseIndex + 1; //offset by one so that <None> is at 0 index
      }

      //Next iterator
      ++traverseIndex;
      it.Next();
    }

    //Duplicate string vector in const char*
    s_itemsPtr.clear();
    for (int i = 0; i < s_items.size(); ++i)
    {
      s_itemsPtr.emplace_back(s_items[i].c_str());
    }

    //Combo UI
    bool changed = ImGui::Combo(text, &currentIndex, s_itemsPtr.data(), s_itemsPtr.size());
    if (changed)
    {
      //<None>
      if (currentIndex == 0)
      {
        *(handle) = Handle<NightEngine::Rendering::Opengl::Material>();
        Debug::Log << "Selected Material: <None>\n";
      }
      //Valid Material
      else
      {
        auto newHandle = CAST_SLOTMAPID_TO_HANDLEOBJECT(Material, s_itemsSlotMapID[currentIndex]);
        if (newHandle.IsValid())
        {
          *(handle) = newHandle;

          auto mat = handle->Get();
          Debug::Log << "Selected Material: " << mat->GetName()
            << " [Path: " << mat->GetFilePath() << "]\n";

          SceneManager::ReregisterAllMeshRenderer();
        }
      }
    }

    return changed;
  }

  MemberSerializerEditor::MemberSerializerEditor(void)
  {
    m_typeEditorMap.insert({ "Handle<Material>",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& dataPtr = variable.GetValue<Handle<Material>>();
      DrawMaterialComboBox("Material", &(dataPtr));
    }
      });

    m_typeEditorMap.insert({ "Material",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& material = variable.GetValue<Material>();
      auto mp = material.GetMaterialProperty();
      if (mp == nullptr)
      {
        return;
      }

      Reflection::Variable var{ METATYPE(Material), &material };

      //Texture Property
      TEXTURE_TABLE(Handle<Texture>)& textureMap = material.GetTextureMap();

      for (auto& pair : textureMap)
      {
        auto channel = Texture::Format::RGB;
        if (pair.second.IsValid() && pair.second->IsValid())
        {
          channel = pair.second->GetInternalFormat();
        }

        Handle<Texture>* tex = &(pair.second);
        DrawTextureComboBox(mp->GetName(pair.first), tex, channel);
      }
      if (textureMap.size() > 0)
      {
        ImGui::Spacing();
        ImGui::Spacing();
      }

      //Vec4 Property
      PROPERTY_TABLE(glm::vec4)& vec4Map = material.GetVec4Map();
      for (auto& pair : vec4Map)
      {
        auto data = mp->GetEditorData(pair.first.c_str());
        void* v4 = &(pair.second);
        IMGUI::DrawVec4Property(data, pair.first.c_str(), (float*)v4);
      }
      if (vec4Map.size() > 0)
      {
        ImGui::Spacing();
        ImGui::Spacing();
      }

      //Float Property
      PROPERTY_TABLE(float)& floatMap = material.GetFloatMap();
      for (auto& pair : floatMap)
      {
        auto data = mp->GetEditorData(pair.first.c_str());
        void* val = &(pair.second);
        IMGUI::DrawFloatProperty(data, pair.first.c_str(), (float*)val);
      }
      if (floatMap.size() > 0)
      {
        ImGui::Spacing();
        ImGui::Spacing();
      }

      //Int/Bool Property
      PROPERTY_TABLE(int)& intMap = material.GetIntMap();
      for (auto& pair : intMap)
      {
        auto data = mp->GetEditorData(pair.first.c_str());
        void* val = &(pair.second);
        //ImGui::InputScalar(pair.first.c_str(), ImGuiDataType_S32, (int*)val);
        IMGUI::DrawIntProperty(data, pair.first.c_str(), (int*)val);
      }
    }
      });


    m_typeEditorMap.insert({ "Vector<Handle<Material>>",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& dataPtr = variable.GetValue<Vector<Handle<Material>>>();
      const auto redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
      const auto greenColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

      if(dataPtr.size() > 0 && ImGui::TreeNode("materials"))
      {
        std::string text = "";
        ImVec4 currColor = greenColor;
        for (int i = 0; i < dataPtr.size(); ++i)
        {
          text = "[" + std::to_string(i) + "] ";
          if (dataPtr[i].IsValid())
          {
            text += dataPtr[i]->GetName().c_str();
            currColor = greenColor;
          }
          else
          {
            text += "Invalid";
            currColor = redColor;
          }
          ImGui::TextColored(currColor, text.c_str());
        }

        ImGui::TreePop();
      }
    }
      });

    ///////////////////////////////////////////////////////

    m_typeEditorMap.insert({ "std::string",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto data = variable.GetValue<std::string>();
      ImGui::Text(data.c_str());
    }
      });
    m_typeEditorMap.insert({ "Color3",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<Color3>();
      ImGui::ColorEdit3(memberName, &data.m_value[0]);
    }
      });
    m_typeEditorMap.insert({ "Color4",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<Color4>();
      ImGui::ColorEdit3(memberName, &data.m_value[0]);
    }
      });
    m_typeEditorMap.insert({ "unsigned long long",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<unsigned long long>();
      ImGui::Text(std::to_string(data).c_str());
    }
      });
    m_typeEditorMap.insert({ "unsigned",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<unsigned>();
      ImGui::Text(std::to_string(data).c_str());
    }
      });
    m_typeEditorMap.insert({ "bool",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<bool>();
      ImGui::Checkbox(memberName, &data);
    }
      });
    m_typeEditorMap.insert({ "GameObject*",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto data = variable.GetValue<GameObject*>();
      ImGui::Text("0x%x", &data);
    }
      });
    m_typeEditorMap.insert({ "Handle<GameObject>",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto data = variable.GetValue<Handle<GameObject>>();
      ImGui::Text(data->GetName().c_str());
    }
      });
    m_typeEditorMap.insert({ "int",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<int>();
      unsigned step = 1u;
      ImGui::InputScalar(memberName, ImGuiDataType_S32, &data,&step);
    }
      });
    m_typeEditorMap.insert({ "MeshRenderer::DrawMode",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<unsigned>();
      unsigned step = 1u;
      ImGui::InputScalar(memberName, ImGuiDataType_U32, &data,&step);
    }
      });
    m_typeEditorMap.insert({ "Light::LightType",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<unsigned>();
      unsigned step = 1u;
      ImGui::InputScalar(memberName, ImGuiDataType_U32, &data,&step);
    }
      });
    m_typeEditorMap.insert({ "Physics::ColliderType",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<int>();
      unsigned step = 1u;
      ImGui::InputScalar(memberName, ImGuiDataType_S32, &data,&step);
    }
      });
    m_typeEditorMap.insert({ "float",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<float>();
      ImGui::DragScalar(memberName, ImGuiDataType_Float, (float*)&data, 0.2f);
    }
      });
    m_typeEditorMap.insert({ "vec2",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<glm::vec2>();
      ImGui::DragFloat2(memberName, (float*)&data, 0.2f);
    }
      });
    m_typeEditorMap.insert({ "vec3",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<glm::vec3>();
      ImGui::DragFloat3(memberName, (float*)&data, 0.05f);
    }
      });
    m_typeEditorMap.insert({ "quat",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<glm::quat>();
      ImGui::DragFloat4(memberName, (float*)&data, 0.2f);
    }
      });

    m_typeEditorMap.insert({ "mat4",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& data = variable.GetValue<glm::mat4>();
      char* num[4] = { "[0]", "[1]", "[2]", "[3]" };
      std::string name;
      ImGui::DragFloat4((name + num[0]).c_str(), (float*)&data[0], 0.2f);
      ImGui::DragFloat4((name + num[1]).c_str(), (float*)&data[1], 0.2f);
      ImGui::DragFloat4((name + num[2]).c_str(), (float*)&data[2], 0.2f);
      ImGui::DragFloat4((name + num[3]).c_str(), (float*)&data[3], 0.2f);
    }
      });
  }

  bool MemberSerializerEditor::DrawMemberEditor(Reflection::Member& member, void* dataObject
    , const std::string& nameMingle)
  {
    using namespace Reflection;
    using namespace Components;

    //lookup typename, do nothing if not found
    auto it = m_typeEditorMap.find(member.GetMetaType()->GetName());
    auto memberPtr = POINTER_OFFSET(dataObject, member.GetOffset());
    if (it != m_typeEditorMap.end())
    {
      Variable var{ member.GetMetaType(), memberPtr };
      it->second(var, (member.GetName() + nameMingle).c_str());
      return true;
    }

    //Try to serialize more Members of member
    auto& mmembers = member.GetMetaType()->GetMembers();
    for (auto& mmember : mmembers)
    {
      ImGui::NextColumn();
      //TYPE
      ImGui::TextColored(g_color_blue
        , mmember.GetMetaType()->GetName().c_str());
      ImGui::NextColumn();

      //Name
      ImGui::Text(mmember.GetName().c_str());
      ImGui::NextColumn();

      //Value Editor
      DrawMemberEditor(mmember, memberPtr);
    }

    return false;
  }

  bool MemberSerializerEditor::DrawMetaTypeEditor(Reflection::MetaType* metaType, void* dataObject
    , const std::string& nameMingle)
  {
    using namespace Reflection;
    using namespace Components;

    //lookup typename, do nothing if not found
    auto it = m_typeEditorMap.find(metaType->GetName());
    auto memberPtr = dataObject;
    if (it != m_typeEditorMap.end())
    {
      Variable var{ metaType, memberPtr };
      it->second(var, (metaType->GetName() + nameMingle).c_str());
      return true;
    }

    ////Try to serialize more Members of member
    //auto& mmembers = metaType->GetMembers();
    //for (auto& mmember : mmembers)
    //{
    //  ImGui::NextColumn();
    //  //TYPE
    //  ImGui::TextColored(g_color_blue
    //    , mmember.GetMetaType()->GetName().c_str());
    //  ImGui::NextColumn();

    //  //Name
    //  ImGui::Text(mmember.GetName().c_str());
    //  ImGui::NextColumn();

    //  //Value Editor
    //  DrawMemberEditor(mmember, memberPtr);
    //} 

    return false;
  }
}