/*!
  @file InstanceDrawer.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of InstanceDrawer
*/
#pragma once

#include <map>
#include <vector>
#include <glm/mat4x4.hpp>

#include "Core/Container/PrimitiveType.hpp"

#include "Graphic/VertexArrayObject.hpp"
#include "Graphic/Mesh.hpp"
#include "Core/EC/Handle.hpp"

namespace Core
{
  namespace ECS
  {
    namespace Components
    {
      //Forward Declaration
      class MeshRenderer;
    }
  }
}

namespace Graphic
{
  class Material;
  class Shader;

  namespace Drawer
  {
    using DrawContainer = std::vector<Core::Factory::HandleObject>;

    enum class DrawPass: unsigned
    {
      BATCH = 0,
      CUSTOM,
      OUTLINE,
      DEBUG
    };

    //! @brief Get Draw Container
    DrawContainer& GetDrawContainer(DrawPass drawPass);

    //! @brief Register the MeshRenderer to the Drawer
    void RegisterMeshRenderer(Core::ECS::Components::MeshRenderer& meshRenderer
      , DrawPass drawPass = DrawPass::BATCH);

    //! @brief Unregister the MeshRenderer to the Drawer
    void UnregisterMeshRenderer(Core::ECS::Components::MeshRenderer& meshRenderer
      , DrawPass drawPass = DrawPass::BATCH);

    //! @brief Draw all registered mesh without binding
    void DrawWithoutBind(Shader& shader
      , DrawPass drawPass = DrawPass::BATCH);

    //! @brief Draw all registered mesh without binding
    void Draw(DrawPass drawPass = DrawPass::BATCH);

    //! @brief Draw pass for Shadow
    void DrawShadowWithoutBind(Shader& shader
      , DrawPass drawPass = DrawPass::BATCH);
  }

  namespace InstanceDrawer
  {
    //! @brief Contain data to be drawn along with its MeshRenderer
    struct InstanceDrawerInfo
    {
      using RendererHandle = Core::Factory::HandleObject;

      std::vector<glm::mat4> m_data;                //Model matrices to be drawn
      std::vector<Mesh>      m_meshes;              //All meshes accociate with meshRenderer

      std::vector<RendererHandle> m_meshrenderers;  //References to Meshrenderer

      //! @brief Add to the reference array, to retrieve their model matrix later
      void AddMeshRenderer(RendererHandle mrHandle);

      //! @brief Buffer all MeshRenderer ModelMatrices
      void Build(void);

      //! @brief Draw all instances of meshes
      void DrawInstances(void);
    };

    //TODO: Rightnow all mesh with same polygon count is the same mesh
    //TODO: A unique Hash for each unique mesh
    struct InstanceSignature
    {
      unsigned int m_shaderID;
      unsigned int m_polygonCount;
    };
    using InstanceMap = std::map<Core::Container::U64, InstanceDrawerInfo>;

    //! @brief Get internal map
    InstanceMap& GetInstanceMap(void);

    //! @brief Register the MeshRenderer to the Drawer
    void RegisterInstance(Core::ECS::Components::MeshRenderer& meshRenderer);

    //! @brief Build all the drawer
    void BuildAllDrawer(void);

    //! @brief Draw all instances registered
    void DrawInstances(Shader& shader);
  }

}