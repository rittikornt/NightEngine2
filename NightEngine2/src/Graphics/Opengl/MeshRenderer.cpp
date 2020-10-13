/*!
  @file MeshRenderer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MeshRenderer
*/
#include "Graphics/Opengl/MeshRenderer.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/EC/SceneManager.hpp"

#include "Graphics/Opengl/Model.hpp"
#include "Graphics/Opengl/Vertex.hpp"
#include "Graphics/Opengl/Shader.hpp"

#include "Graphics/Opengl/InstanceDrawer.hpp"

#include "Core/Serialization/ResourceManager.hpp"

using namespace NightEngine::Rendering::Opengl;

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      INIT_REFLECTION_AND_FACTORY(MeshRenderer, 2000, 1000)

        void MeshRenderer::InitMesh(const std::vector<NightEngine::Rendering::Opengl::Vertex>& vertices
          , const std::vector<unsigned>& indices
          , bool castShadow, bool buildNow)
      {
        // Cache <MeshSignature, MeshData> that doesn't bind to opengl state yet
        m_castShadow = castShadow;
        m_meshes.emplace_back(vertices
          , indices, buildNow);

        m_submeshCount = (unsigned)m_meshes.size();
      }

      void MeshRenderer::InitMesh(const Vertex* vertices, size_t vertexArraySize
        , const unsigned* indices, size_t indexArraySize
        , bool castShadow
        , bool buildNow)
      {
        //TODO: Load mesh data from ResourceManager cache
        // Cache <MeshSignature, MeshData> that doesn't bind to opengl state yet
        m_castShadow = castShadow;
        m_meshes.emplace_back(vertices, vertexArraySize
          , indices, indexArraySize, buildNow);

        m_submeshCount = (unsigned)m_meshes.size();
      }

      void MeshRenderer::ReregisterDrawMode(void)
      {
        auto defaultMat = SceneManager::GetDefaultMaterial();
        bool validMat = m_material.IsValid();
        bool shouldReregister = validMat
          && m_drawMode != DrawMode::DEBUG
          && m_drawMode != DrawMode::STATIC;

        //Don't register draw call with invalid material
        if (shouldReregister)
        {
          if (defaultMat.m_handle == m_material.m_handle
            && !m_useModelLoadedMaterials)
          {
            RegisterDrawMode(DrawMode::PREBIND);
          }
          else
          {
            RegisterDrawMode(DrawMode::CUSTOM);
          }
        }
      }

      void MeshRenderer::RegisterDrawMode(DrawMode mode)
      {
        //Unregister old drawmode
        if (m_drawMode != DrawMode::UNINITIALIZED)
        {
          UnregisterDrawMode(m_drawMode);
        }

        //Set mode
        m_drawMode = mode;

        ASSERT_TRUE(m_handle.IsValid());

        //Register To Drawer
        switch (m_drawMode)
        {
        case DrawMode::STATIC:
        {
          ASSERT_MSG(m_material.IsValid()
            , "m_material is required to register instanceDrawer");

          GPUInstancedDrawer::RegisterInstance(*this);
          break;
        }
        case DrawMode::PREBIND:
        {
          Drawer::RegisterMeshRenderer(*this
            , Drawer::DrawPass::BATCH);
          break;
        }
        case DrawMode::CUSTOM:
        {
          auto dp = Drawer::DrawPass::OPAQUE_PASS;
          Drawer::RegisterMeshRenderer(*this, dp);
          break;
        }
        case DrawMode::OUTLINE:
        {
          Drawer::RegisterMeshRenderer(*this
            , Drawer::DrawPass::OUTLINE);
          break;
        }
        case DrawMode::DEBUG:
        {
          Drawer::RegisterMeshRenderer(*this
            , Drawer::DrawPass::DEBUG);
          break;
        }
        case DrawMode::DISABLE:
        {
          break;
        }
        }
      }

      void MeshRenderer::UnregisterDrawMode(DrawMode mode)
      {
        //Unregister from Drawer
        switch (mode)
        {
        case DrawMode::STATIC:
        {
          GPUInstancedDrawer::UnregisterInstance(*this);
          break;
        }
        case DrawMode::PREBIND:
        {
          Drawer::UnregisterMeshRenderer(*this
            , Drawer::DrawPass::BATCH);
          break;
        }
        case DrawMode::CUSTOM:
        {
          auto dp = Drawer::DrawPass::OPAQUE_PASS;
          Drawer::UnregisterMeshRenderer(*this, dp);
          break;
        }
        case DrawMode::OUTLINE:
        {
          Drawer::UnregisterMeshRenderer(*this
            , Drawer::DrawPass::OUTLINE);
          break;
        }
        case DrawMode::DEBUG:
        {
          Drawer::UnregisterMeshRenderer(*this
            , Drawer::DrawPass::DEBUG);
          break;
        }
        case DrawMode::DISABLE:
        {
          break;
        }
        }
      }

      glm::mat4 MeshRenderer::GetModelMatrix(void)
      {
        //Get Transform's model matrix
        auto t = m_gameObject->GetTransform();
        ASSERT_TRUE(t != nullptr);

        return t->CalculateModelMatrix();
      }

      void MeshRenderer::DrawWithMaterial(ShaderUniformsFn fn)
      {
        if (m_useModelLoadedMaterials)
        {
          auto errorMat = SceneManager::GetErrorMaterial();
          Handle<Material> currMat;

          for (size_t i = 0; i < m_meshes.size(); ++i)
          {
            currMat = i < m_materials.size() 
              && m_materials[i].IsValid() ? m_materials[i] : errorMat;
            currMat->Bind(true);
            {
              //SetUniform Modelmatrix
              auto t = m_gameObject->GetTransform();
              ASSERT_TRUE(t != nullptr);
              currMat->GetShader().SetUniform("u_model", t->GetModelMatrix());
              
              if (fn != nullptr)
              {
                fn(currMat->GetShader());
              }

              m_meshes[i].Draw();
            }
            currMat->Unbind();
          }
        }
        else
        {
          //If for some reason material become invalid (deleted), assign this mesh an error material
          if (!m_material.IsValid())
          {
            m_material = SceneManager::GetErrorMaterial();
            ASSERT_TRUE(m_material.IsValid());
          }

          m_material->Bind(true);
          {
            //SetUniform Modelmatrix
            auto t = m_gameObject->GetTransform();
            ASSERT_TRUE(t != nullptr);
            m_material->GetShader().SetUniform("u_model", t->GetModelMatrix());

            if (fn != nullptr)
            {
              fn(m_material->GetShader());
            }

            //Draw meshes
            for (size_t i = 0; i < m_meshes.size(); ++i)
            {
              m_meshes[i].Draw();
            }
          }
          m_material->Unbind();
        }
      }

      void MeshRenderer::DrawMeshes(void)
      {
        //Draw meshes
        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
          m_meshes[i].Draw();
        }
      }

      void MeshRenderer::DrawWithoutBind(bool useTexture, NightEngine::Rendering::Opengl::Shader& shader)
      {
        //SetUniform Modelmatrix
        auto t = m_gameObject->GetTransform();
        ASSERT_TRUE(t != nullptr);
        shader.SetUniform("u_model", t->GetModelMatrix());

        //Draw
        DrawMeshes();
      }

      void MeshRenderer::DrawWithoutBindDepthPass(bool useTexture, NightEngine::Rendering::Opengl::Shader& shader)
      {
        //SetUniform Modelmatrix
        auto t = m_gameObject->GetTransform();
        ASSERT_TRUE(t != nullptr);
        shader.SetUniform("u_model", t->GetModelMatrix());
        shader.SetUniform("u_prevModel", t->GetPrevModelMatrix());

        //Skip Transparent Material in Depth Prepass
        //Draw meshes
        if (m_useModelLoadedMaterials)
        {
          for (size_t i = 0; i < m_meshes.size(); ++i)
          {
            bool isTransparent = i < m_materials.size()
              && m_materials[i].IsValid() && !m_materials[i]->IsOpaque();
            
            if(!isTransparent)
            {
              m_meshes[i].Draw();
            }
          }
        }
        else
        {
          bool isTransparent = m_material.IsValid() 
            && !m_material->IsOpaque();

          if (!isTransparent)
          {
            for (size_t i = 0; i < m_meshes.size(); ++i)
            {
              m_meshes[i].Draw();
            }
          }
        }
      }

      void MeshRenderer::DrawWithMode(bool useTexture, NightEngine::Rendering::Opengl::Shader& shader)
      {
        ASSERT_TRUE(m_drawMode != DrawMode::UNINITIALIZED);

        switch (m_drawMode)
        {
        case DrawMode::PREBIND:
        {
          //SetUniform Modelmatrix
          auto t = m_gameObject->GetTransform();
          ASSERT_TRUE(t != nullptr);
          shader.SetUniform("u_model", t->GetModelMatrix());

          //Draw
          DrawMeshes();
          break;
        }
        case DrawMode::CUSTOM:
        {
          //Draw with setted material
          DrawWithMaterial();

          //Bind Shader back
          shader.Bind();
          break;
        }
        case DrawMode::OUTLINE:
        {
          //TODO: Redo Outline Shader
          //Clear Stencil
          glStencilMask(0xFF);  //enable write
          glClear(GL_STENCIL_BUFFER_BIT);

          //All drawn fragment will set bit to stencil buffer
          //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
          glStencilFunc(GL_ALWAYS, 1, 0xFF);
          auto t = m_gameObject->GetTransform();
          {
            //Draw Normally
            //SetUniform Modelmatrix
            ASSERT_TRUE(t != nullptr);
            shader.SetUniform("u_model", t->GetModelMatrix());

            //Draw
            DrawMeshes();
          }
          shader.Unbind();

          // (Outline Part)
          //Only draw on 0 value, without writing to stencil
          //But write to depth buffer too
          glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
          glStencilMask(0x00);
          //glDisable(GL_DEPTH_TEST);
          {
            m_material->Bind(useTexture);
            {
              //SetUniform Modelmatrix with scaled up
              auto modelMatrix = t->CalculateModelMatrix(t->GetPosition()
                , t->GetRotation(), t->GetScale() * 1.1f);
              m_material->GetShader().SetUniform("u_model", modelMatrix);

              //Draw
              DrawMeshes();
            }
            m_material->Unbind();
          }

          //Bind Shader back
          shader.Bind();

          //Turn depth test back on
          glEnable(GL_DEPTH_TEST);

          //Always pass default
          glStencilFunc(GL_ALWAYS, 1, 0xFF);
          break;
        }
        }
      }

      void MeshRenderer::LoadModel(const std::string& path
        , bool buildNow, bool castShadow)
      {
        m_meshLoadPath = path;
        m_castShadow = castShadow;

        //Load the model through ResourceManager
        auto model = ResourceManager::LoadModelResource(path);
        ASSERT_TRUE(model != nullptr);

        m_meshes = model->GetMeshes();
        m_submeshCount = (unsigned)m_meshes.size();

        // Get Materials from Model if there are all materials for each submesh
        // and all valid materials
        if (model->IsValidMaterials())
        {
          auto& loadedMaterials = model->GetMaterials();
          m_materials = loadedMaterials;
          m_useModelLoadedMaterials = true;
        }

        if (buildNow)
        {
          for (auto& mesh : m_meshes)
          {
            mesh.Build();
          }
        }
      }

      unsigned MeshRenderer::GetPolygonCount(void) const
      {
        unsigned sum = 0;
        for (auto& mesh : m_meshes)
        {
          sum += mesh.GetPolygonCount();
        }
        return sum;
      }

      void MeshRenderer::OnDestroy(void)
      {
        UnregisterDrawMode(m_drawMode);

        //Release all meshes
        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
          m_meshes[i].Release();
        }

        m_material.m_handle.Nullify();

        m_useModelLoadedMaterials = false;
        m_submeshCount = 0;
        m_materials.clear();
        m_meshes.clear();

        m_meshLoadPath = "";
        m_castShadow = false;
        m_drawMode = DrawMode::UNINITIALIZED;
      }

      void MeshRenderer::LoadMaterial(std::string fileName)
      {
        //Load Material
        m_material = Material::LoadMaterial(fileName);
      }

      bool MeshRenderer::IsOpaque(void)
      {
        if (m_material.IsValid()
          && !m_material->IsOpaque())
        {
          return false;
        }

        for (int i = 0; i < m_materials.size(); ++i)
        {
          if (m_materials[i].IsValid()
            && !m_materials[i]->IsOpaque())
          {
            return false;
          }
        }
        return true;
      }

      ///////////////////////////////////////////////////////

      void MeshRenderer::OnStartFrame(void)
      {
        auto t = m_gameObject->GetTransform();
        t->CalculateModelMatrix();
      }

      void MeshRenderer::OnEndFrame(void)
      {
        auto t = m_gameObject->GetTransform();
        t->SavePreviousModelMatrix();
      }
    }
  }
}