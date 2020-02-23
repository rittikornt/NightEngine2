/*!
  @file MeshRenderer.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of MeshRenderer
*/
#include "Core/EC/Components/MeshRenderer.hpp"
#include "Core/EC/GameObject.hpp"

#include "Graphics/Opengl/Model.hpp"
#include "Graphics/Opengl/Vertex.hpp"
#include "Graphics/Opengl/Shader.hpp"

#include "Graphics/Opengl/InstanceDrawer.hpp"

#include "Core/Serialization/ResourceManager.hpp"

using namespace Rendering;

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      INIT_REFLECTION_FOR(MeshRenderer)

      void MeshRenderer::InitMesh(const std::vector<Rendering::Vertex>& vertices
        , const std::vector<unsigned>& indices
        , bool castShadow, bool buildNow)
      {
        // Cache <MeshSignature, MeshData> that doesn't bind to opengl state yet
        m_castShadow = castShadow;
        m_meshes.emplace_back(vertices
          , indices, buildNow);

        m_meshCount = m_meshes.size();
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

        m_meshCount = m_meshes.size();
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
            ASSERT_MSG(m_material != nullptr
            , "m_material is required to register instanceDrawer");

            InstanceDrawer::RegisterInstance(*this);
            break;
          }
          case DrawMode::NORMAL:
          {
            Drawer::RegisterMeshRenderer(*this
              , Drawer::DrawPass::BATCH);
            break;
          }
          case DrawMode::CUSTOM:
          {
            Drawer::RegisterMeshRenderer(*this
              , Drawer::DrawPass::CUSTOM);
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
          case DrawMode::NORMAL:
          {
            Drawer::UnregisterMeshRenderer(*this
              , Drawer::DrawPass::BATCH);
            break;
          }
          case DrawMode::CUSTOM:
          {
            Drawer::UnregisterMeshRenderer(*this
              , Drawer::DrawPass::CUSTOM);
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

      void MeshRenderer::DrawWithMaterial(void)
      {
        ASSERT_TRUE(m_material != nullptr);

        m_material->Bind(true);
        {
          //SetUniform Modelmatrix
          auto t = m_gameObject->GetTransform();
          ASSERT_TRUE(t != nullptr);
          m_material->GetShader().SetUniform("u_model", t->CalculateModelMatrix());

          //Draw meshes
          for (size_t i = 0; i < m_meshes.size(); ++i)
          {
            m_meshes[i].Draw();
          }
        }
        m_material->Unbind();
      }

      void MeshRenderer::DrawMeshes(void)
      {
        //Draw meshes
        for (size_t i = 0; i < m_meshes.size(); ++i)
        {
          m_meshes[i].Draw();
        }
      }

      void MeshRenderer::DrawWithoutBind(bool useTexture, Rendering::Shader& shader)
      {
        //SetUniform Modelmatrix
        auto t = m_gameObject->GetTransform();
        ASSERT_TRUE(t != nullptr);
        shader.SetUniform("u_model", t->CalculateModelMatrix());

        //Draw
        DrawMeshes();
      }

      void MeshRenderer::DrawWithMode(bool useTexture, Rendering::Shader& shader)
      {
        ASSERT_TRUE(m_drawMode != DrawMode::UNINITIALIZED);

        switch (m_drawMode)
        {
          case DrawMode::NORMAL:
          {
            //SetUniform Modelmatrix
            auto t = m_gameObject->GetTransform();
            ASSERT_TRUE(t != nullptr);
            shader.SetUniform("u_model", t->CalculateModelMatrix());

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
              shader.SetUniform("u_model", t->CalculateModelMatrix());

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

      void MeshRenderer::LoadModel(const std::string & path
        , bool buildNow, bool castShadow)
      {
        m_castShadow = castShadow;

        //Load the model through ResourceManager
        auto model = ResourceManager::LoadModelResource(path);
        ASSERT_TRUE(model != nullptr);

        //Model model{ path };
        m_meshes = model->GetMeshes();
        m_meshCount = m_meshes.size();

        if (buildNow)
        {
          for(auto& mesh: m_meshes)
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
      }

      void MeshRenderer::LoadMaterial(std::string fileName)
      {
        //Load Material
        m_material = Material::LoadMaterial(fileName);
      }
    }
  }
}