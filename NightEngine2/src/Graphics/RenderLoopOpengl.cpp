/*!
  @file GraphicCore.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderLoopOpengl
*/
#include "Graphics/RenderLoopOpengl.hpp"
#include "Graphics/Opengl/Window.hpp"
#include "Graphics/Editor.hpp"

#include "Graphics/Opengl/PrimitiveShape.hpp"
#include "Graphics/Opengl/InstanceDrawer.hpp"

//TODO: Remove these later
#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/Model.hpp"
#include "Graphics/Opengl/Light.hpp"
#include "Graphics/Opengl/OpenglAllocationTracker.hpp"
#include "Graphics/Opengl/ShaderTracker.hpp"

#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"
#include "Graphics/Opengl/DebugMarker.hpp"
#include "Graphics/Opengl/RenderState.hpp"

//GameObject
#include "Graphics/Opengl/MeshRenderer.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Factory.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"

#include "Core/EC/SceneManager.hpp"

//Subsystem
#include "Core/Serialization/ResourceManager.hpp"
#include "Core/Logger.hpp"
#include "Core/Serialization/FileSystem.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Input/Input.hpp"
#include "Physics/PhysicsScene.hpp"

// System Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/anim.h>

//Engine defines
#include "EngineConfigs.hpp"

using namespace NightEngine;
using namespace NightEngine::Factory;
using namespace NightEngine::EC;
using namespace NightEngine::EC::Components;

using namespace NightEngine::Rendering::Opengl::PrimitiveShape;
using namespace NightEngine::Rendering::Opengl::Postprocess;
using namespace NightEngine::Rendering::Opengl;

namespace NightEngine::Rendering::Opengl
{
  void SceneBuffer::LazyInit(CameraObject& camera, GBuffer& gbuffer)
  {
    if (m_resolution.x == 0)
    {
      m_resolution.x = camera.m_scaledPixelResolution.x;
      m_resolution.y = camera.m_scaledPixelResolution.y;

      m_sceneTexture = Texture::GenerateRenderTexture(m_resolution.x, m_resolution.y
        , Texture::Format::RGBA16F, Texture::PixelFormat::RGBA
        , Texture::FilterMode::LINEAR
        , Texture::WrapMode::CLAMP_TO_EDGE);
      m_sceneTexture.SetName("SceneColorRT");

      //Scene FBO
      m_sceneFbo.Init();
      m_sceneFbo.AttachColorTexture(m_sceneTexture);
      m_sceneFbo.AttachDepthTexture(gbuffer.m_depthTexture);
      m_sceneFbo.Bind();

      //Fullscreen Mesh
      m_screenTriangleVAO.Init();
      m_screenTriangleVAO.Build(BufferMode::Static, Triangle::vertices
        , Triangle::indices, Triangle::info);
    }
    else
    {
      int width = camera.m_scaledPixelResolution.x;
      int height = camera.m_scaledPixelResolution.y;

      // Check for resize
      if (m_resolution.x != width || m_resolution.y != height)
      {
        m_resolution.x = width;
        m_resolution.y = height;
        m_sceneTexture.Resize(width, height, Texture::PixelFormat::RGBA);

        /*glBindTexture(GL_TEXTURE_2D, m_sceneTexture.GetID());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);*/
      }
    }
  }
}

namespace NightEngine::Rendering
{
  const std::string   g_lightSpaceMatrices[]{
     "u_lightSpaceMatrices[0]","u_lightSpaceMatrices[1]"
    ,"u_lightSpaceMatrices[2]","u_lightSpaceMatrices[3]"
    ,"u_lightSpaceMatrices[4]" ,"u_lightSpaceMatrices[5]" };

  static SceneLights g_sceneLights;
  static float g_time = 0.0f;
  static glm::vec3 g_cameraPosition = glm::vec3(0.0f);

  static bool g_enablePostprocess = true;
  static bool g_showLight = true;
  static glm::mat4   g_dirLightWorldToLightSpaceMatrix;

  static int g_dirLightResolution = 2048;
  static int g_pointLightResolution = 1024;

  //*********************************************
  // Helper Functions
  //*********************************************
  static void ClearLightData(Shader& shader)
  {
    static const std::string g_pointLightStr[] =
    { "u_pointLightInfo[0]", "u_pointLightInfo[1]", "u_pointLightInfo[2]"
     , "u_pointLightInfo[3]", "u_pointLightInfo[4]" };

    static const std::string g_spotLightStr[] =
    { "u_spotLightInfo[0]", "u_spotLightInfo[1]", "u_spotLightInfo[2]"
      , "u_spotLightInfo[3]", "u_spotLightInfo[4]" };

    shader.SetUniform("u_dirLightInfo.m_intensity", 0.0f);
    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
    {
      shader.SetUniform(g_pointLightStr[i] + ".m_intensity", 0.0f);
    }

    for (int i = 0; i < SPOTLIGHT_AMOUNT; ++i)
    {
      shader.SetUniform(g_spotLightStr[i] + ".m_intensity", 0.0f);
    }
  }

  static void ApplyLight(Shader& shader)
  {
    //Light Apply loop
    auto& lightContainer = Factory::GetTypeContainer<Light>();
    {
      auto it = lightContainer.GetIterator();
      while (!it.IsEnd())
      {
        auto light = it.Get();
        light->ApplyLightInfo(shader);

        it.Next();
      }
    }
  }

  static void DrawLoop(Shader& shader)
  {
    //MeshRenderer Draw Loop
    auto& meshRendererContainer = Factory::GetTypeContainer<MeshRenderer>();
    auto it = meshRendererContainer.GetIterator();
    while (!it.IsEnd())
    {
      auto mr = it.Get();
      mr->DrawWithoutBind(false, shader);

      it.Next();
    }
  }

  //*********************************************
  // RenderLoopOpengl
  //*********************************************
  void RenderLoopOpengl::Initialize(void)
  {
    Debug::Log << "NightEngine::Rendering::Initialize\n";

    //************************************************
    // Opengl Setting
    //************************************************
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); //So Cubemap drawn with DepthValue == 1.0
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //Set stencil value if it is 0
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0x00);  //Disable write

    glDisable(GL_BLEND);

    //Face culling
    glEnable(GL_CULL_FACE);

#if(EDITOR_MODE)
    Editor::Initialize();
#endif

    //************************************************
    // Frame Buffer Object
    //************************************************
    //m_width = Window::GetWidth(); m_height = Window::GetHeight();
    m_camera.SetResolution(Window::GetWidth(), Window::GetHeight());
    int width = m_camera.m_scaledPixelResolution.x;
    int height = m_camera.m_scaledPixelResolution.y;

    //Depth FBO for shadow
    //m_initResolution.x = (float)m_width, m_initResolution.y = (float)m_height;

    m_depthDirShadowFBO.Init();
    m_shadowMapDirShadowTexture = m_depthDirShadowFBO.CreateAndAttachDepthTexture(g_dirLightResolution, g_dirLightResolution);

    m_depthDirShadowMaterial.InitShader("RenderPass/Shadows/depth_directional.vert"
      , "RenderPass/Shadows/depth_directional.frag");

    //Depth FBO for point shadow
    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
    {
      m_depthPointShadowFBO[i].Init();

      m_shadowMapPointShadow[i].InitDepthCubemap((unsigned)g_pointLightResolution);
      m_depthPointShadowFBO[i].AttachCubemap(m_shadowMapPointShadow[i]);
    }

    m_depthPointShadowMaterial.InitShader("RenderPass/Shadows/depth_point.vert"
      , "RenderPass/Shadows/depth_point.frag", "RenderPass/Shadows/depth_point.geom");

    //GBuffer
    m_gbuffer.LazyInit(m_camera);
    m_depthPrepass.Init(m_gbuffer);

    //Scene Texture
    m_sceneBuffer.LazyInit(m_camera, m_gbuffer);

    //************************************************
    // Postprocess
    //************************************************
    m_postProcessSetting = &(SceneManager::GetPostProcessSetting());
    m_postProcessSetting->LazyInit(m_camera, m_gbuffer);

    //Prepass
    m_cameraMotionVector.Init(m_gbuffer);

    //************************************************
    // Cubemap
    //************************************************
    //IBL
    m_ibl.Init(m_camera, m_sceneBuffer.m_screenTriangleVAO);

    //************************************************
    // Material
    //************************************************
    m_defaultMaterial = (SceneManager::GetDefaultMaterial());
    m_defaultMaterial->InitShader(DEFAULT_VERTEX_SHADER_PBR
      , DEFAULT_FRAG_SHADER_PBR);
    m_defaultMaterial->SetName("DefaultMaterial");

    m_defaultMaterial->InitPBRTexture(FileSystem::GetFilePath("diffuse_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , true, FileSystem::GetFilePath("normal_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , "", "", FileSystem::GetFilePath("emissive_wood.png", FileSystem::DirectoryType::Textures)
      , false, "");

    m_lightingMaterial.InitShader("Utility/fullscreenTriangle.vert"
      , "Lighting/deferred_lighting_pbr_pass.frag");
    SetDeferredLightingPassUniforms(m_lightingMaterial);

    m_debugViewMaterial.InitShader("Utility/fullscreenTriangle.vert"
      , "Lighting/deferred_pbr_debug_view.frag");
    SetDeferredLightingPassUniforms(m_debugViewMaterial);

    //Debug material
    m_billboardMaterial = (SceneManager::GetBillBoardMaterial());
    m_billboardMaterial->InitShader("ShaderPass/Debug/debug_billboard.vert"
      , "ShaderPass/Debug/debug_fragment.frag");
    m_billboardMaterial->SetName("BillboardMaterial");

    m_billboardMaterial->Bind(false);
    m_billboardMaterial->GetShader().SetUniform("u_color", glm::vec3(1.0f));
    m_billboardMaterial->Unbind();

    m_lightTexture = Texture(FileSystem::GetFilePath("Icon_Light.png", FileSystem::DirectoryType::Textures)
      , Texture::Format::RGBA);

    Material::PreLoadAllMaterials();

    //************************************************
    // Setup Box, Model GameObjects
    //************************************************
    {
      std::vector<glm::mat4> modelMat;
      for (int i = 0; i < 10; i++)
      {
        modelMat.push_back(
          Transform::CalculateModelMatrix(glm::vec3(i, 10.0f, 4.0f)
            , glm::quat(), glm::vec3(1.0f)));
      }
    }

    //************************************************
    // Uniform Buffer Object
    //************************************************
    int bindingPoint = 0;
    m_defaultMaterial->GetShader().SetUniformBlockBindingPoint("u_matrices", bindingPoint);
    m_uniformBufferObject.Init(sizeof(glm::mat4) * 2, bindingPoint);
    m_uniformBufferObject.FillBuffer(sizeof(glm::mat4), sizeof(glm::mat4)
      , glm::value_ptr(m_camera.GetUnjitteredProjectionMatrix()));
  }

  void RenderLoopOpengl::Terminate(void)
  {
    Debug::Log << "NightEngine::Rendering::Terminate\n";

    g_sceneLights.Clear();

    //TODO: Remove later
    //Manuall Clean up of all GameObject
    {
      auto& container = NightEngine::Factory::GetTypeContainer<GameObject>();
      size_t size = container.Size();
      auto& _array = container.GetArray();

      for (int i = 0; i < _array.size(); ++i)
      {
        if (_array[i].first.m_active)
        {
          _array[i].second.UnsubscribeAll();
          _array[i].second.RemoveAllComponents();
          _array[i].second.Destroy();
        }
      }
    }

    GPUInstancedDrawer::UnregisterAllInstances();

#if(EDITOR_MODE)
    Editor::Terminate();
#endif

    //Clean up all the loaded gl object and all the cached data in ResourceManager
    OpenglAllocationTracker::DeallocateAllLoadedObjects();
    NightEngine::ResourceManager::ClearAllData();

    SceneManager::DeletePostProcessSetting();

    Window::Terminate();
    OpenglAllocationTracker::PrintAllocationState();
    ShaderTracker::Clear();
  }

  void RenderLoopOpengl::Render(float dt)
  {
    g_time += dt;
    g_time = fmodf(g_time, FLT_MAX);

    Opengl::CameraObject::ProcessCameraInput(m_camera, dt);

    //Update the new Projection Matrix
    m_camera.m_bJitterProjectionMatrix = m_postProcessSetting->m_taaPP.m_enable;
    m_camera.m_camSize.m_fov = cameraFOV;
    m_camera.m_far = cameraFarPlane;
    m_camera.m_jitterStrength = m_postProcessSetting->m_taaPP.m_frustumJitterStrength;

    m_camera.OnStartFrame();
    Drawer::OnStartFrame(Drawer::DrawPass::UNDEFINED);
    Drawer::OnStartFrame(Drawer::DrawPass::OPAQUE_PASS);
    Drawer::OnStartFrame(Drawer::DrawPass::DEBUG);

    //Update View/Projection matrix to Shader
    m_uniformBufferObject.FillBuffer(0, sizeof(glm::mat4)
      , glm::value_ptr(m_camera.m_VP));
    m_uniformBufferObject.FillBuffer(sizeof(glm::mat4), sizeof(glm::mat4)
      , glm::value_ptr(m_camera.m_projection));

    //Dynamically Resize Texture if needed
    {
      m_sceneBuffer.LazyInit(m_camera, m_gbuffer);
      m_gbuffer.LazyInit(m_camera);
      m_postProcessSetting->LazyInit(m_camera, m_gbuffer);
    }

    //Update global variable for lambda func
    g_cameraPosition = m_camera.m_position;

    //*************************************************
    // Rendering Loop
    //*************************************************
    if (!Window::ShouldClose())
    {
#if(EDITOR_MODE)
      Editor::PreRender();

      DebugMarker::PushDebugGroup("Render");
      {
        Render();
      }
      DebugMarker::PopDebugGroup();

      DebugMarker::PushDebugGroup("ImGUI Editor");
      {
        Editor::PostRender();
      }
      DebugMarker::PopDebugGroup();

      DebugMarker::EndFrame();
#else
      Render();
#endif

      // Flip Buffers and Draw
      Window::SwapBuffer();
      glfwPollEvents();
    }

    m_camera.OnEndFrame();
    Drawer::OnEndFrame(Drawer::DrawPass::OPAQUE_PASS);
    Drawer::OnEndFrame(Drawer::DrawPass::UNDEFINED);
  }

  void RenderLoopOpengl::OnRecompiledShader(void)
  {
    //Geometry Pass
    m_defaultMaterial->RefreshTextureUniforms();
    ResourceManager::RefreshMaterialTextureUniforms();

    //Lighting Pass
    SetDeferredLightingPassUniforms(m_lightingMaterial);
    SetDeferredLightingPassUniforms(m_debugViewMaterial);
    
    //IBL
    m_ibl.RefreshTextureUniforms(m_camera);

    //Postprocessing
    m_postProcessSetting->RefreshTextureUniforms();
  }

  /////////////////////////////////////////////////////////////

  void RenderLoopOpengl::Render(void)
  {
    //Clear BackBuffer Color and Depth
    const float clear_color = 0.0f;
    glClearColor(clear_color, clear_color, clear_color, clear_color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float pointShadowFarPlane = m_camera.m_far;

    //*************************************************
    // Depth Prepass
    //*************************************************
    m_depthPrepass.Execute(m_camera);

    //*************************************************
    // ShadowCaster Prepass
    //*************************************************
    //TODO: Make into its own ShadowCasterPass struct/class
    DebugMarker::PushDebugGroup("ShadowCaster Pass");
    {
      //*************************************************
      // Depth FBO Pass for directional light shadow
      //*************************************************
      glViewport(0, 0, (GLsizei)g_dirLightResolution, (GLsizei)g_dirLightResolution);
      glEnable(GL_DEPTH_TEST);

      //TODO: don't refresh lights component every frame
      SceneManager::GetLights(g_sceneLights);

      //Shader and Matrices
      DebugMarker::PushDebugGroup("DirectionalLight ShadowCaster Pass");
      if (g_sceneLights.dirLights.size() > 0)
      {
        auto lightComponent = g_sceneLights.dirLights[0]->GetComponent("Light");
        g_dirLightWorldToLightSpaceMatrix = lightComponent->Get<Light>()
          ->CalculateDirLightWorldToLightSpaceMatrix(m_camera, mainShadowsSize, 0.3f, mainShadowsFarPlane);

        //Draw pass to FBO
        m_depthDirShadowFBO.Bind();
        {
          glClear(GL_DEPTH_BUFFER_BIT);
          m_depthDirShadowMaterial.Bind(false);
          {
            m_depthDirShadowMaterial.GetShader().SetUniform("u_lightSpaceMatrix"
              , g_dirLightWorldToLightSpaceMatrix);

            //Draw all Mesh with depthMaterial
            Drawer::DrawShadowWithoutBind(m_depthDirShadowMaterial.GetShader()
              , Drawer::DrawPass::UNDEFINED);
            Drawer::DrawShadowWithoutBind(m_depthDirShadowMaterial.GetShader()
              , Drawer::DrawPass::OPAQUE_PASS);
          }
          m_depthDirShadowMaterial.Unbind();
        }
        m_depthDirShadowFBO.Unbind();
      }
      DebugMarker::PopDebugGroup();

      //*************************************************
      // Depth FBO Pass for point shadow
      //*************************************************
      glViewport(0, 0, (GLsizei)g_pointLightResolution, (GLsizei)g_pointLightResolution);
      if (g_sceneLights.pointLights.size() > 0)
      {
        for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
        {
          //Shader and Matrices
          auto pointLightComponent = g_sceneLights.pointLights[i]->GetComponent("Light");
          auto& lightSpaceMatrices = pointLightComponent->Get<Light>()
            ->CalculatePointLightWorldToLightSpaceMatrices(90.0f, 1.0f, 0.1f, pointShadowFarPlane);

          //Draw to FBO
          DebugMarker::PushDebugGroup("PointLight ShadowCaster Pass");
          m_depthPointShadowFBO[i].Bind();
          {
            glClear(GL_DEPTH_BUFFER_BIT);
            //Depth Material
            m_depthPointShadowMaterial.Bind(false);
            {
              for (int i = 0; i < 6; ++i)
              {
                m_depthPointShadowMaterial.GetShader().SetUniform(g_lightSpaceMatrices[i]
                  , lightSpaceMatrices[i]);
              }
              m_depthPointShadowMaterial.GetShader().SetUniform("u_lightPos"
                , g_sceneLights.pointLights[i]->GetTransform()->GetPosition());
              m_depthPointShadowMaterial.GetShader().SetUniform("u_farPlane"
                , pointShadowFarPlane);

              //Draw all Mesh with depthMaterial
              Drawer::DrawShadowWithoutBind(m_depthPointShadowMaterial.GetShader()
                , Drawer::DrawPass::UNDEFINED);
              Drawer::DrawShadowWithoutBind(m_depthPointShadowMaterial.GetShader()
                , Drawer::DrawPass::OPAQUE_PASS);
            }
            m_depthPointShadowMaterial.Unbind();
          }
          m_depthPointShadowFBO[i].Unbind();
          DebugMarker::PopDebugGroup();
        }
      }
    }
    DebugMarker::PopDebugGroup();

    //*************************************************
    // Geometry Pass
    //*************************************************
    //glViewport(0, 0, (GLsizei)m_initResolution.x, (GLsizei)m_initResolution.y);
    glViewport(0, 0, m_camera.m_scaledPixelResolution.x, m_camera.m_scaledPixelResolution.y);
    
    // This got weird real fast lol
    m_gbuffer.Execute(m_defaultMaterial
      , [](NightEngine::EC::Handle<Material>& defaultMaterial)
      {
        //Set Uniform
        defaultMaterial->Bind();
        {
          Shader& shader = defaultMaterial->GetShader();
          shader.SetUniform("u_lightSpaceMatrix", g_dirLightWorldToLightSpaceMatrix);

          //Draw Static Instances
          GPUInstancedDrawer::DrawInstances(shader);

          //Draw Loop by traversing Containers
          Drawer::DrawWithoutBind(shader, Drawer::DrawPass::UNDEFINED);
        }
        defaultMaterial->Unbind();

        //Draw Custom Pass
        Drawer::Draw(Drawer::DrawPass::OPAQUE_PASS
          , [](Shader& shader)
          {
            shader.SetUniform("u_lightSpaceMatrix", g_dirLightWorldToLightSpaceMatrix);
            //shader.SetUniformNoErrorCheck("u_cameraPosWS", g_cameraPosition);
          });
      });

    //*************************************************
    // Camera Motion Vector Pass
    //*************************************************
    DebugMarker::PushDebugGroup("CameraMotionVector");
    {
      m_cameraMotionVector.Render(m_sceneBuffer.m_screenTriangleVAO
        , m_gbuffer, m_camera);
    }
    DebugMarker::PopDebugGroup();

    //*************************************************
    // Lighting Pass
    //*************************************************
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    RenderSetup::PassStencilIfBitSet(RenderFeature::GBUFFER_MASK);

    //TODO: Move into a separate Lighting Pass struct/class
    DebugMarker::PushDebugGroup("Deferred Lighting Pass");
    m_sceneBuffer.m_sceneFbo.Bind();
    {
      //Clear Buffer
      glClear(GL_COLOR_BUFFER_BIT);

      //Set Material Uniform
      bool debugView = IsDebugView();
      Material& lightingPassMat = debugView ? m_debugViewMaterial: m_lightingMaterial;
      
      lightingPassMat.Bind(false);
      {
        Shader& shader = lightingPassMat.GetShader();
        shader.SetUniform("u_farPlane", pointShadowFarPlane);
        shader.SetUniform("u_ambientStrength", ambientStrength);
        shader.SetUniform("u_invVP", m_camera.m_invVP);
        shader.SetUniform("u_lightSpaceMatrix", g_dirLightWorldToLightSpaceMatrix);
        
        if (debugView)
        {
          shader.SetUniform("u_debugViewIndex", (int)m_debugView);
          shader.SetUniform("u_debugShadowViewIndex", (int)m_debugShadowView);
          
          shader.SetUniform("u_motionVector", 4);
          m_gbuffer.m_motionVector.BindToTextureUnit(4);
        }

        //Shadow 2D texture
        m_shadowMapDirShadowTexture.BindToTextureUnit(5);

        //Shadow Cubemap texture
        for (int j = 0; j < POINTLIGHT_AMOUNT; ++j)
        {
          m_shadowMapPointShadow[j].BindToTextureUnit(6 + j);
        }

        //IBL
        m_ibl.m_irradianceCubemap.BindToTextureUnit(10);
        m_ibl.m_prefilterMap.BindToTextureUnit(11);
        m_ibl.m_brdfLUT.BindToTextureUnit(12);
        m_gbuffer.m_depthTexture.BindToTextureUnit(13);

        //Bind Gbuffer Texture
        m_gbuffer.BindTextures();

        //Camera Position
        m_camera.ApplyCameraInfo(shader);

        //Apply Light information to the Shader
        ClearLightData(shader);
        ApplyLight(shader);

        //Draw Fullscreen Mesh
        m_sceneBuffer.m_screenTriangleVAO.Draw();
      }
      lightingPassMat.Unbind();

      glDisable(GL_STENCIL_TEST);

      //Draw Cubemap
      if (m_debugView != DebugView::MAIN_SHADOW_DEPTH)
      {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_ibl.DrawCubemap(IBL::CubemapType::SKYBOX, m_camera);
      }
    }
    m_sceneBuffer.m_sceneFbo.Unbind();
    DebugMarker::PopDebugGroup();

    //*************************************************
    // PostProcess Pass
    //*************************************************
    if (Input::GetKeyDown(Input::KeyCode::KEY_8))
    {
      g_enablePostprocess = !g_enablePostprocess;
      if (!g_enablePostprocess)
      {
        m_postProcessSetting->Clear();
      }
    }

    if (g_enablePostprocess)
    {
      DebugMarker::PushDebugGroup("PostProcess");
      //Postfx
      {
        m_postProcessSetting->Apply(PostProcessContext{ &m_camera, &m_gbuffer
          , & m_sceneBuffer.m_sceneFbo, & m_sceneBuffer.m_screenTriangleVAO, & m_sceneBuffer.m_sceneTexture
          , g_time, screenZoomScale });
      }
      DebugMarker::PopDebugGroup();
    }
    else
    { 
      //Blit to backbuffer
      m_sceneBuffer.m_sceneFbo.CopyBufferToTarget(m_camera.m_scaledPixelResolution.x, m_camera.m_scaledPixelResolution.y
        , m_camera.m_windowPixelResolution.x, m_camera.m_windowPixelResolution.y
        , 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    //Debugging View
    if (Input::GetKeyDown(Input::KeyCode::KEY_7))
    {
      g_showLight = !g_showLight;
    }

    //*************************************************
    // Draw Debug Icons
    //*************************************************
    DrawDebugIcons();
  }

  void RenderLoopOpengl::DrawDebugIcons()
  {
    DebugMarker::PushDebugGroup("Editor Icons");
    {
      glViewport(0, 0, m_camera.m_windowPixelResolution.x
        , m_camera.m_windowPixelResolution.y);
      if (g_showLight)
      {
        glEnable(GL_DEPTH_TEST);

        Texture::SetBlendMode(true);
        //Draw Light Icons Billboard
        m_camera.ApplyViewMatrix(m_billboardMaterial->GetShader());
        m_billboardMaterial->Bind(false);
        {
          Shader& shader = m_billboardMaterial->GetShader();

          shader.SetUniform("u_projection", m_camera.m_unjitteredProjection);
          shader.SetUniform("u_color", glm::vec3(1.0f, 1.0f, 1.0f));

          shader.SetUniform("u_texture", 0);
          m_lightTexture.BindToTextureUnit(0);

          Drawer::DrawWithoutBind(shader
            , Drawer::DrawPass::DEBUG);
        }
        m_billboardMaterial->Unbind();
        Texture::SetBlendMode(false);
      }

      // Physics Debug Draw
      glClear(GL_DEPTH_BUFFER_BIT);
      auto physicsScene = Physics::PhysicsScene::GetPhysicsScene(0);
      if (physicsScene != nullptr)
      {
        physicsScene->DebugDraw(m_camera);
      }
    }
    DebugMarker::PopDebugGroup();
  }

  void RenderLoopOpengl::SetDeferredLightingPassUniforms(Opengl::Material& material)
  {
    material.Bind(false);
    {
      Shader& shader = material.GetShader();

      //Directional Shadow
      shader.SetUniform("u_shadowMap2D", 5);
      m_shadowMapDirShadowTexture.BindToTextureUnit(5);

      //Point Shadow
      shader.SetUniform("u_shadowMap[0]", 6);
      shader.SetUniform("u_shadowMap[1]", 7);
      shader.SetUniform("u_shadowMap[2]", 8);
      shader.SetUniform("u_shadowMap[3]", 9);

      //IBL
      shader.SetUniform("u_irradianceMap", 10);
      shader.SetUniform("u_prefilterMap", 11);
      shader.SetUniform("u_brdfLUT", 12);

      //Gbuffer's texture
      m_gbuffer.RefreshTextureUniforms(shader);
    }
    material.Unbind();
  }

} // Rendering
