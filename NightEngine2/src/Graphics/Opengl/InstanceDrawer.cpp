/*!
  @file InstanceDrawer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of InstanceDrawer
*/

#include "Graphics/Opengl/InstanceDrawer.hpp"
#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Mesh.hpp"
#include "Graphics/Opengl/Shader.hpp"

#include "Core/Macros.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"
#include "Core/Utility/Utility.hpp"

#include "Core/EC/Factory.hpp"
#include "Core/Container/MurmurHash2.hpp"

using namespace NightEngine;
using namespace NightEngine::Container;
using namespace NightEngine::EC::Components;

namespace Rendering
{
  namespace Drawer
  {
    DrawContainer& GetDrawContainer(DrawPass drawPass)
    {
      static std::map<DrawPass, DrawContainer> container;

      auto& it = container.find(drawPass);
      if (it != container.end())
      {
        return it->second;
      }

      //Insert new DrawPass
      container.insert({ drawPass, DrawContainer() });
      return container[drawPass];
    }

    void RegisterMeshRenderer(MeshRenderer& meshRenderer
      , DrawPass drawPass)
    {
      auto& container = GetDrawContainer(drawPass);

      auto handle = meshRenderer.GetHandle();
      ASSERT_TRUE(handle.Get<MeshRenderer>() != nullptr);

      container.emplace_back(handle);
    }

    void UnregisterMeshRenderer(NightEngine::EC::Components::MeshRenderer& meshRenderer
      , DrawPass drawPass)
    {
      auto& container = GetDrawContainer(drawPass);

      auto handle = meshRenderer.GetHandle();
      ASSERT_TRUE(handle.Get<MeshRenderer>() != nullptr);

      //Remove the handle from Drawer
      for (auto it = container.begin()
        ; it != container.end(); )
      {
        if (*it == handle)
        {
          it = container.erase(it);
          return;
        }
        else
        {
          ++it;
        }
      }
    }

    void DrawWithoutBind(Shader& shader
      , DrawPass drawPass)
    {
      auto& container = GetDrawContainer(drawPass);

      //Profiled, this loop is as fast as direct Slotmap lookup
      for (auto& mesh : container)
      {
        mesh.Get<MeshRenderer>()->DrawWithoutBind(false, shader);
      }
    }

    void Draw(DrawPass drawPass, ShaderUniformsFn fn)
    {
      auto& container = GetDrawContainer(drawPass);

      //Profiled, this loop is as fast as direct Slotmap lookup
      for (auto& mesh : container)
      {
        mesh.Get<MeshRenderer>()->DrawWithMaterial(fn);
      }
    }

    void DrawShadowWithoutBind(Shader& shader
      , DrawPass drawPass)
    {
      auto& container = GetDrawContainer(drawPass);

      //Profiled, this loop is as fast as Slotmap lookup directly
      for (auto& mesh : container)
      {
        auto mr = mesh.Get<MeshRenderer>();
        if (mr->IsCastingShadow())
        {
          mr->DrawWithoutBind(false, shader);
        }
      }
    }
  }

  /////////////////////////////////////////////////////////////

  namespace GPUInstancedDrawer
  {
    void BatchInfo::AddMeshRenderer(RendererHandle mrHandle)
    {
      m_meshrenderers.emplace_back(mrHandle);

      //Copy Mesh data to draw
      if (m_meshes.size() == 0)
      {
        m_meshes = mrHandle.Get<MeshRenderer>()->GetMeshes();
      }
    }

    void BatchInfo::Build(void)
    {
      //Save model matrix of each meshRenderer
      for (auto& handle : m_meshrenderers)
      {
        auto mr = handle.Get<MeshRenderer>();
        ASSERT_TRUE(mr != nullptr);
        m_data.emplace_back(mr->GetModelMatrix());
      }

      //Buffer modelmatrices into Opengl Buffer
      for (auto& mesh : m_meshes)
      {
        mesh.BuildInstancesDraw(m_data.size() * sizeof(glm::mat4)
          , &m_data[0]);
      }
    }

    void BatchInfo::DrawInstances(void)
    {
      //TODO: Set Uniform for u_model to represent all the object's position

      for (auto& mesh : m_meshes)
      {
        mesh.DrawInstanced(m_data.size());
      }
    }

    DrawBatchMap& GetInternalDrawBatchMap(void)
    {
      static DrawBatchMap map;
      return map;
    }

    void RegisterInstance(MeshRenderer& meshRenderer)
    {
      auto& map = GetInternalDrawBatchMap();

      //TODO: Signature that make more sense
      //(Material + MeshName?) as Signature
      auto& meshes = meshRenderer.GetMeshes();
      auto  mat = meshRenderer.GetMaterial();
      InstanceSignature signature{ mat->GetShader().GetProgramID()
      , meshRenderer.GetPolygonCount() };

      const char* ptr = reinterpret_cast<const char*>(&signature);
      U64 key = NightEngine::Container::ConvertToHash(ptr, sizeof(InstanceSignature));

      //Initialize new Batch
      auto it = map.find(key);
      if (it == map.end())
      {
        map.insert({ key, BatchInfo() });
      }

      //Add mesh renderer to the Batch
      map[key].AddMeshRenderer(meshRenderer.GetHandle());
    }

    void UnregisterInstance(NightEngine::EC::Components::MeshRenderer& meshRenderer)
    {
      auto& map = GetInternalDrawBatchMap();

      //TODO: Signature that make more sense
      //(Material + MeshName?) as Signature
      auto& meshes = meshRenderer.GetMeshes();
      auto  mat = meshRenderer.GetMaterial();
      if (mat != nullptr)
      {
        InstanceSignature signature{ mat->GetShader().GetProgramID()
        , meshRenderer.GetPolygonCount() };

        const char* ptr = reinterpret_cast<const char*>(&signature);
        U64 key = NightEngine::Container::ConvertToHash(ptr, sizeof(InstanceSignature));

        //Initialize
        auto it = map.find(key);
        if (it != map.end())
        {
          auto mrHandle = meshRenderer.GetHandle();
          Debug::Log << "GPUInstancedDrawer:UnregisterInstance[" << mrHandle.Get<MeshRenderer>()->GetUID() << "]\n";

          auto& batchInfo = it->second;
          auto& meshHandles = batchInfo.m_meshrenderers;
          auto& datas = batchInfo.m_data;
          auto& meshes = batchInfo.m_meshes;

          bool erased = false;
          int dataIndex = 0;
          for (auto it2 = meshHandles.begin();
            it2 != meshHandles.end(); ++it2, ++dataIndex)
          {
            if (*it2 == mrHandle)
            {
              meshHandles.erase(it2);
              datas.erase(datas.begin() + dataIndex);
              erased = true;
              break;
            }
          }

          if (erased && datas.size() == 0)
          {
            //Deallocate the VAO instanced Buffer
            for (auto& mesh : meshes)
            {
              mesh.Release();
            }
            meshes.clear();

            //Remove this empty batch
            map.erase(key);
          }
        }
      }
    }

    void UnregisterAllInstances(void)
    {
      auto& map = GetInternalDrawBatchMap();
      map.clear();
    }

    void BuildAllDrawer(void)
    {
      auto& map = GetInternalDrawBatchMap();
      for (auto it = map.begin()
        ; it != map.end(); ++it)
      {
        it->second.Build();
      }
    }

    void DrawInstances(Shader& shader)
    {
      ASSERT_MSG(shader.IsValidUniform("u_instanceRendering")
      , "Shader doesn't support Instances Rendering");
      shader.SetUniform("u_instanceRendering", true);

      auto& map = GetInternalDrawBatchMap();
      for (auto it = map.begin()
        ; it != map.end(); ++it)
      {
        it->second.DrawInstances();
      }

      shader.SetUniform("u_instanceRendering", false);
    }
  }

}