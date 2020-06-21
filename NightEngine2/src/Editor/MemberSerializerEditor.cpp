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

#include "Graphics/Opengl/Window.hpp"

#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Graphics/Color.hpp"
#include <unordered_map>

#include "Core/Utility/Utility.hpp"
#include "Core/Utility/Profiling.hpp"

using namespace NightEngine::EC;
using namespace NightEngine;
using namespace Rendering;

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);
  static bool g_dirty = false;

  void MemberSerializerEditor::NewFrame(void)
  {
    g_dirty = true;
  }

  static bool DrawTextureComboBox(const char* text, Handle<Rendering::Texture>* handle, Texture::Channel channel)
  {
    const char* k_none = "None";
    static int m_currentItem = 0;
    static std::vector <std::string> s_items;
    static std::unordered_map <std::string, int> s_itemsMap;
    static std::vector <const char*> s_itemsPtr;
    static std::string  s_texFilePath;

    //Find all Texture name list only once per frame
    if (g_dirty)
    {
      s_items.clear();
      s_items.emplace_back(k_none);

      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Assets, s_items
        , FileSystem::FileFilter::FullPath, ".png", false, true, false);
      FileSystem::GetAllFilesInDirectory(FileSystem::DirectoryType::Assets, s_items
        , FileSystem::FileFilter::FullPath, ".jpg", false, true, false);

      s_itemsMap.clear();
      for (int i = 0; i < s_items.size(); ++i)
      {
        s_itemsMap.insert({ s_items[i] , i });
      }
      g_dirty = true;
    }

    m_currentItem = 0;
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
      m_currentItem = it->second;
    }

    //Combo Box Imgui
    bool changed = ImGui::Combo(text, &m_currentItem, s_itemsPtr.data(), s_itemsPtr.size());
    if (changed)
    {
      if (s_items[m_currentItem] != k_none)
      {
        auto newHandle = Texture::LoadTextureHandle(s_items[m_currentItem]
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

  static bool DrawMaterialComboBox(const char* text, Handle<Rendering::Material>* handle)
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
        *(handle) = Handle<Rendering::Material>();
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
      auto& dataPtr = variable.GetValue<Material>();
      float min = 0.0f, max = 1.0f, nmax = 50.0f;

      //Diffuse
      Reflection::Variable var{ METATYPE(Material), &dataPtr };
      auto memberPtr = var.GetMetaType()->FindMember("m_diffuseColor");
      void* ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::ColorEdit3(memberName, (float*)ptr, ImGuiColorEditFlags_HDR);

      memberPtr = var.GetMetaType()->FindMember("m_diffuseTexture");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      DrawTextureComboBox("Diffuse Texture", static_cast<Handle<Texture>*>(ptr)
        , Texture::Channel::SRGB);

      //Normal
      memberPtr = var.GetMetaType()->FindMember("m_normalTexture");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      DrawTextureComboBox("Normal Texture", static_cast<Handle<Texture>*>(ptr)
        , Texture::Channel::RGB);

      memberPtr = var.GetMetaType()->FindMember("m_normalMultiplier");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Normal Multiplier", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &nmax);

      memberPtr = var.GetMetaType()->FindMember("m_useNormal");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      bool& bptr = *((bool*)ptr);
      ImGui::Checkbox("Use Normal", &bptr);

      //Roughness
      memberPtr = var.GetMetaType()->FindMember("m_roughnessTexture");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      DrawTextureComboBox("Roughness Texture", static_cast<Handle<Texture>*>(ptr)
        , Texture::Channel::RGB);

      memberPtr = var.GetMetaType()->FindMember("m_roughnessValue");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Roughness Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      //Metallic
      memberPtr = var.GetMetaType()->FindMember("m_metallicTexture");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      DrawTextureComboBox("Metallic Texture", static_cast<Handle<Texture>*>(ptr)
        , Texture::Channel::RGB);

      memberPtr = var.GetMetaType()->FindMember("m_metallicValue");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Metallic Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      //Emissive
      memberPtr = var.GetMetaType()->FindMember("m_emissiveTexture");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      DrawTextureComboBox("Emissive Texture", static_cast<Handle<Texture>*>(ptr)
        , Texture::Channel::RGB);

      memberPtr = var.GetMetaType()->FindMember("m_emissiveStrength");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Emissive Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min);
    }
      });

    /*m_typeEditorMap.insert({ "Material*",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto dataPtr = variable.GetValue<Material*>();
      if (dataPtr == nullptr)
      {
        ImGui::Text("nullptr");
        return;
      }
      float min = 0.0f, max = 1.0f, nmax = 50.0f;

      Reflection::Variable var{ METATYPE(Material), dataPtr };
      auto memberPtr = var.GetMetaType()->FindMember("m_diffuseColor");
      void* ptr = POINTER_OFFSET(&(*dataPtr), memberPtr->GetOffset());
      ImGui::ColorEdit3(memberName, (float*)ptr, ImGuiColorEditFlags_HDR);

      memberPtr = var.GetMetaType()->FindMember("m_normalMultiplier");
      ptr = POINTER_OFFSET(&(*dataPtr), memberPtr->GetOffset());
      ImGui::DragScalar("Normal Multiplier", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &nmax);

      memberPtr = var.GetMetaType()->FindMember("m_useNormal");
      ptr = POINTER_OFFSET( &(*dataPtr), memberPtr->GetOffset());
      bool& bptr = *((bool*)ptr);
      ImGui::Checkbox("Use Normal", &bptr);

      memberPtr = var.GetMetaType()->FindMember("m_roughnessValue");
      ptr = POINTER_OFFSET(&(*dataPtr), memberPtr->GetOffset());
      ImGui::DragScalar("Roughness Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      memberPtr = var.GetMetaType()->FindMember("m_metallicValue");
      ptr = POINTER_OFFSET(&(*dataPtr), memberPtr->GetOffset());
      ImGui::DragScalar("Metallic Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      memberPtr = var.GetMetaType()->FindMember("m_emissiveStrength");
      ptr = POINTER_OFFSET(&(*dataPtr), memberPtr->GetOffset());
      ImGui::DragScalar("Emissive Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min);
    }
    });*/

    //TODO: turn these into Macros?
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