#include "GBuffer.hpp"
/*!
  @file DepthPrepass.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of DepthPrepass
*/

#include "Graphics/Opengl/RenderPass/DepthPrepass.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/CameraObject.hpp"

#include "Graphics/Opengl/DebugMarker.hpp"
#include "Graphics/Opengl/InstanceDrawer.hpp"

#include <glad/glad.h>

using namespace NightEngine;

namespace Rendering
{
  void DepthPrepass::Init(int width, int height, GBuffer& gbuffer)
  {
    m_width = width, m_height = height;

    //Material
    m_depthPrepassMaterial.InitShader("ShaderPass/depth_prepass.vert"
      , "ShaderPass/depth_prepass.frag");

    //FBO
    m_fbo.Init();
    m_fbo.AttachDepthTexture(gbuffer.m_depthTexture);
    m_fbo.AttachColorTexture(gbuffer.GetTexture(GBufferTarget::MotionVector));
    CHECKGL_ERROR();

    m_fbo.Bind();
    m_fbo.Unbind();

    RefreshTextureUniforms();
  }

  void DepthPrepass::Execute(CameraObject& camera)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    auto resolution = camera.GetScreenSize();
    glViewport(0, 0, (GLsizei)resolution.x, (GLsizei)resolution.y);

    DebugMarker::PushDebugGroup("Depth Prepass and Object Motion Vector");
    m_fbo.Bind();
    {
      glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      //Draw Depth prepass
      {
        auto& shader = m_depthPrepassMaterial.GetShader();
        shader.Bind();
        {
          shader.SetUniform("u_unjitteredVP", camera.m_unjitteredVP);
          shader.SetUniform("u_prevUnjitteredVP", camera.m_prevUnjitteredVP);

          //Draw Static Instances
          GPUInstancedDrawer::DrawInstances(shader);

          //Draw Loop by traversing Containers
          Drawer::DrawWithoutBind(shader, Drawer::DrawPass::BATCH);

          //TODO: Do per object motion vector here too
          //Should skip meshRenderer that has u_useOpacityMap flag to handle alpha cutoff properly
          //Draw Custom Pass
          Drawer::DrawDepthWithoutBind(shader, Drawer::DrawPass::OPAQUE_PASS);
        }
        shader.Unbind();
      }
    }
    m_fbo.Unbind();
    DebugMarker::PopDebugGroup();
  }

  void DepthPrepass::RefreshTextureUniforms()
  {
  }
}