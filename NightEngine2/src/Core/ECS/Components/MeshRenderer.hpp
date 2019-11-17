/*!
  @file MeshRenderer.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MeshRenderer
*/
#pragma once

#include "Core/ECS/ComponentLogic.hpp"
#include "Core/ECS/Handle.hpp"
#include "Graphic/Material.hpp"
#include "Graphic/Mesh.hpp"

#include <vector>

namespace Graphic
{
  //Forward Declaration
  class Shader;
}

namespace Core
{
  namespace ECS
  {
    namespace Components
    {
      class MeshRenderer: public ComponentLogic
      {
        REFLECTABLE_TYPE();
        public:
          enum class DrawMode : unsigned
          {
            NORMAL = 0,     //Draw with prebinded material
            CUSTOM ,        //Draw with m_material
            OUTLINE,
            DEBUG,
            DISABLE,
            STATIC,         //Static, enable instance Drawing
            UNINITIALIZED   //Invalid 
          };

          //! @brief Init from mesh information
          void InitMesh(const std::vector<Graphic::Vertex>& vertices
            , const std::vector<unsigned>& indices
            , bool castShadow = true
            , bool buildNow = true);

          //! @brief Init from mesh information
          void InitMesh(const Graphic::Vertex* vertices, size_t vertexArraySize
            , const unsigned* indices, size_t indexArraySize
            , bool castShadow = true
            , bool buildNow = true);

          //! @brief Register to Drawer mode
          void RegisterDrawMode(DrawMode mode);

          //! @brief Unregister from Drawer for specific mode
          void UnregisterDrawMode(DrawMode mode);

          //! @brief Set Material
          void SetMaterial(Graphic::Material* material) { m_material = material; }

          //! @brief Get Material
          Graphic::Material* GetMaterial(void) { return m_material; }

          //! @brief Get Material
          glm::mat4 GetModelMatrix(void);

          //! @brief Get Meshes
          const std::vector<Graphic::Mesh>& GetMeshes(void) { return m_meshes; }

          //! @brief Check if casting shadow
          bool IsCastingShadow(void) { return m_castShadow; }

          //! @brief Draw mesh with custom m_material
          void DrawWithMaterial(void);

          //! @brief Plain draw loop
          void DrawMeshes(void);

          //! @brief Draw mesh without binding material
          void DrawWithoutBind(bool useTexture, Graphic::Shader& shader);

          //! @brief Draw mesh based on mode
          void DrawWithMode(bool useTexture, Graphic::Shader& shader);

          //! @brief Loading Model from path
          void LoadModel(const std::string& path
            , bool buildNow, bool castShadow = true);

          //! @brief Get all mesh polygon count
          unsigned GetPolygonCount(void) const;

          //! @brief On destroy callback
          virtual void OnDestroy(void);

          //! @brief Load Material from file
          void LoadMaterial(std::string fileName);
        private:
          //TODO: use handle instead of pointer
          Graphic::Material*           m_material;

          //TODO: Vector<Handle<Mesh>> instead
          std::vector<Graphic::Mesh>   m_meshes;
          unsigned                     m_meshCount = 0;

          bool                         m_castShadow = true;
          DrawMode                     m_drawMode = DrawMode::UNINITIALIZED;
      };
    }
  }
}