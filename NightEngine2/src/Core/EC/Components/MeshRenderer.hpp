/*!
  @file MeshRenderer.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MeshRenderer
*/
#pragma once

#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Handle.hpp"
#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Mesh.hpp"

#include <vector>

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      class MeshRenderer: public ComponentLogic
      {
        REFLECTABLE_TYPE_BLOCK()
        {
          META_REGISTERER_WITHBASE(MeshRenderer, ComponentLogic
            , InheritType::PUBLIC, true
            , nullptr, nullptr)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_material, true)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_materials, false)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_useModelLoadedMaterials, true)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_drawMode, true)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_submeshCount, true)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_meshLoadPath, true)
            .MR_ADD_MEMBER_PROTECTED(MeshRenderer, m_castShadow, true);
        }
        public:
          enum class DrawMode : unsigned
          {
            PREBIND = 0,     //Draw with prebinded material
            CUSTOM ,        //Draw with m_material
            OUTLINE,
            DEBUG,
            DISABLE,
            STATIC,         //Static, enable instance Drawing
            UNINITIALIZED   //Invalid 
          };

          //! @brief Init from mesh information
          void InitMesh(const std::vector<Rendering::Vertex>& vertices
            , const std::vector<unsigned>& indices
            , bool castShadow = true
            , bool buildNow = true);

          //! @brief Init from mesh information
          void InitMesh(const Rendering::Vertex* vertices, size_t vertexArraySize
            , const unsigned* indices, size_t indexArraySize
            , bool castShadow = true
            , bool buildNow = true);

          //! @brief Reregister Draw mode
          void ReregisterDrawMode(void);

          //! @brief Register to Drawer mode
          void RegisterDrawMode(DrawMode mode);

          //! @brief Unregister from Drawer for specific mode
          void UnregisterDrawMode(DrawMode mode);

          //! @brief Set Material
          void SetMaterial(EC::Handle<Rendering::Material> material) { m_material = material; }

          //! @brief Get Material
          Rendering::Material* GetMaterial(void) { return m_material.IsValid()?m_material.Get(): nullptr; }

          //! @brief Get DrawMode
          DrawMode GetDrawMode(void) const { return m_drawMode; }

          //! @brief Get Material
          glm::mat4 GetModelMatrix(void);

          //! @brief Get Load Path
          const std::string& GetMeshLoadPath(void) const { return m_meshLoadPath; }

          //! @brief Get Meshes
          const std::vector<Rendering::Mesh>& GetMeshes(void) { return m_meshes; }

          //! @brief Check if casting shadow
          bool IsCastingShadow(void) { return m_castShadow; }

          //! @brief Draw mesh with custom m_material
          void DrawWithMaterial(Rendering::ShaderUniformsFn fn = nullptr);

          //! @brief Plain draw loop
          void DrawMeshes(void);

          //! @brief Draw mesh without binding material
          void DrawWithoutBind(bool useTexture, Rendering::Shader& shader);

          //! @brief Draw mesh without binding material
          void DrawWithoutBindDepthPass(bool useTexture, Rendering::Shader& shader);

          //! @brief Draw mesh based on mode
          void DrawWithMode(bool useTexture, Rendering::Shader& shader);

          //! @brief Loading Model from path
          void LoadModel(const std::string& path
            , bool buildNow, bool castShadow = true);

          //! @brief Get all mesh polygon count
          unsigned GetPolygonCount(void) const;

          //! @brief On destroy callback
          virtual void OnDestroy(void);

          //! @brief Load Material from file
          void LoadMaterial(std::string fileName);

          //! @brief Check if this mesh is opaque
          bool IsOpaque(void);

          ///////////////////////////////////////////////////////

          //! @brief Start Rendering frame
          void OnStartFrame(void);

          //! @brief End Rendering frame
          void OnEndFrame(void);

        private:
          EC::Handle<Rendering::Material> m_material;

          std::vector <EC::Handle<Rendering::Material>> m_materials;
          bool                            m_useModelLoadedMaterials = false;
          bool                            m_isOpaque = true;

          std::vector<Rendering::Mesh>    m_meshes; //All the submeshes
          unsigned                        m_submeshCount = 0;
          std::string                     m_meshLoadPath;

          bool                            m_castShadow = true;
          DrawMode                        m_drawMode = DrawMode::UNINITIALIZED;
      };
    }
  }
}