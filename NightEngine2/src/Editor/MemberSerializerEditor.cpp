/*!
  @file MemberSerializerEditor.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MemberSerializerEditor
*/

#include "Editor/MemberSerializerEditor.hpp"
#include "imgui/imgui.h"

#include "Core/Reflection/ReflectionMacros.hpp"

#include "Core/EC/GameObject.hpp"
#include "Graphics/Opengl/Window.hpp"

#include "Graphics/Color.hpp"

using namespace NightEngine::EC;
using namespace NightEngine;
using namespace Rendering;

namespace Editor
{
  static ImVec4 g_color_blue = ImVec4(0.165f, 0.6f, 1.0f, 1.0f);

  MemberSerializerEditor::MemberSerializerEditor(void)
  {

    m_typeEditorMap.insert({ "Handle<Material>",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto dataPtr = variable.GetValue<Handle<Material>>();
      auto mat = dataPtr.Get();

    }
    });

    m_typeEditorMap.insert({ "Material",
      [](Reflection::Variable& variable, const char* memberName)
    {
      auto& dataPtr = variable.GetValue<Material>();
      float min = 0.0f, max = 1.0f, nmax = 50.0f;

      Reflection::Variable var{ METATYPE(Material), &dataPtr };
      auto memberPtr = var.GetMetaType()->FindMember("m_diffuseColor");
      void* ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::ColorEdit3(memberName, (float*)ptr, ImGuiColorEditFlags_HDR);

      memberPtr = var.GetMetaType()->FindMember("m_normalMultiplier");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Normal Multiplier", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &nmax);

      memberPtr = var.GetMetaType()->FindMember("m_useNormal");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      bool& bptr = *((bool*)ptr);
      ImGui::Checkbox("Use Normal", &bptr);

      memberPtr = var.GetMetaType()->FindMember("m_roughnessValue");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Roughness Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      memberPtr = var.GetMetaType()->FindMember("m_metallicValue");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Metallic Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min, &max);

      memberPtr = var.GetMetaType()->FindMember("m_emissiveStrength");
      ptr = POINTER_OFFSET(&dataPtr, memberPtr->GetOffset());
      ImGui::DragScalar("Emissive Value", ImGuiDataType_Float, (float*)ptr, 0.05f, &min);
    }
    });

    m_typeEditorMap.insert({ "Material*",
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
    });

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