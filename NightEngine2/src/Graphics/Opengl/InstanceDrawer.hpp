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

#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Mesh.hpp"
#include "Graphics/Opengl/Shader.hpp"
#include "Core/EC/Handle.hpp"

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      //Forward Declaration
      class MeshRenderer;
    }
  }
}

namespace NightEngine::Rendering::Opengl
{
  class Material;

  namespace Drawer
  {
    using DrawContainer = std::vector<NightEngine::EC::HandleObject>;

    enum class DrawPass: unsigned
    {
      UNDEFINED = 0,
      OPAQUE_PASS,
      OUTLINE,
      DEBUG
    };

    //! @brief Get Draw Container
    DrawContainer& GetDrawContainer(DrawPass drawPass);

    //! @brief Register the MeshRenderer to the Drawer
    void RegisterMeshRenderer(NightEngine::EC::Components::MeshRenderer& meshRenderer
      , DrawPass drawPass = DrawPass::UNDEFINED);

    //! @brief Unregister the MeshRenderer to the Drawer
    void UnregisterMeshRenderer(NightEngine::EC::Components::MeshRenderer& meshRenderer
      , DrawPass drawPass = DrawPass::UNDEFINED);

    //! @brief Draw all registered mesh without binding
    void DrawWithoutBind(Shader& shader
      , DrawPass drawPass = DrawPass::UNDEFINED);

    //! @brief Draw all registered mesh without binding
    void Draw(DrawPass drawPass = DrawPass::UNDEFINED
      , NightEngine::Rendering::Opengl::ShaderUniformsFn fn = nullptr);

    //! @brief Draw pass for Shadow
    void DrawShadowWithoutBind(Shader& shader
      , DrawPass drawPass = DrawPass::UNDEFINED);

    //! @brief Draw pass for Shadow
    void DrawDepthWithoutBind(Shader& shader
      , DrawPass drawPass = DrawPass::UNDEFINED);

    /////////////////////////////////////////////////////////////

    //! @brief On Start Rendering Frame
    void OnStartFrame(DrawPass drawPass = DrawPass::UNDEFINED);

    //! @brief On End Rendering Frame
    void OnEndFrame(DrawPass drawPass = DrawPass::UNDEFINED);
  }

  namespace GPUInstancedDrawer
  {
    //! @brief Contain data to be drawn along with its MeshRenderer
    struct BatchInfo
    {
      using RendererHandle = NightEngine::EC::HandleObject;

      std::vector<glm::mat4> m_data;                //Model matrices to be drawn
      std::vector<Mesh>      m_meshes;              //All meshes accociate with meshRenderer

      std::vector<RendererHandle> m_meshrenderers;  //References to Meshrenderer

      //! @brief Add to the reference array, to retrieve their model matrix later
      void AddMeshRenderer(RendererHandle mrHandle);

      //! @brief Buffer all MeshRenderer ModelMatrices to GPU
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
    using DrawBatchMap = std::map<NightEngine::Container::U64, BatchInfo>;

    //! @brief Get internal Draw batch map map<U64 signature, DrawInfo>
    DrawBatchMap& GetInternalDrawBatchMap(void);

    //! @brief Register the MeshRenderer to the Drawer
    void RegisterInstance(NightEngine::EC::Components::MeshRenderer& meshRenderer);

    //! @brief Unregister the MeshRenderer from the Drawer
    void UnregisterInstance(NightEngine::EC::Components::MeshRenderer& meshRenderer);

    //! @brief Unregister all the MeshRenderer from the Drawer
    void UnregisterAllInstances(void);

    //! @brief Build all the drawer
    void BuildAllDrawer(void);

    //! @brief Draw all instances registered
    void DrawInstances(Shader& shader);
  }

}