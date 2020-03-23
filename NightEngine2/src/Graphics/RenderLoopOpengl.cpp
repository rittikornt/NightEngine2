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
#include "Graphics/ShaderTracker.hpp"

#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"

//GameObject
#include "Core/EC/Components/MeshRenderer.hpp"
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

#define EDITOR_MODE true

using namespace NightEngine;
using namespace NightEngine::Factory;
using namespace NightEngine::EC;
using namespace NightEngine::EC::Components;

using namespace Rendering::PrimitiveShape;
using namespace Rendering::Postprocess;

//TODO: Remove bunch of nastiness away from this file
// Got to start from properly serialize the Scene file (Save/Load)
namespace Rendering
{
  const std::string   g_lightSpaceMatrices[]{
     "u_lightSpaceMatrices[0]","u_lightSpaceMatrices[1]"
    ,"u_lightSpaceMatrices[2]","u_lightSpaceMatrices[3]"
    ,"u_lightSpaceMatrices[4]" ,"u_lightSpaceMatrices[5]" };

  //Camera
  static CameraObject g_camera{ CameraObject::CameraType::PERSPECTIVE
    ,103.0f };

  static SceneLights g_sceneLights;

  //*********************************************
  // Helper Functions
  //*********************************************
  void ProcessInput(float dt)
  {
    using namespace Input;
    static float moveSpeed = 4.0f;
    static const float walkSpeed = 4.0f;
    static const float runSpeed = 10.0f;

    static float mouseSpeed = 10.0f;
    static float rotateSpeed = 50.0f;

    if (Input::GetMouseHold(MouseKeyCode::MOUSE_BUTTON_RIGHT))
    {
      if (Input::GetKeyHold(KeyCode::KEY_LEFT_SHIFT))
      {
        moveSpeed = runSpeed;
      }
      else
      {
        moveSpeed = walkSpeed;
      }
      if (Input::GetKeyHold(KeyCode::KEY_W))
      {
        g_camera.Move(glm::vec3(0.0f, 0.0f, moveSpeed *dt));
      }
      if (Input::GetKeyHold(KeyCode::KEY_S))
      {
        g_camera.Move(glm::vec3(0.0f, 0.0f, -moveSpeed * dt));
      }
      if (Input::GetKeyHold(KeyCode::KEY_D))
      {
        g_camera.Move(glm::vec3(moveSpeed*dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_A))
      {
        g_camera.Move(glm::vec3(-moveSpeed * dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_Q))
      {
        g_camera.Move(glm::vec3(0.0f, -moveSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_E))
      {
        g_camera.Move(glm::vec3(0.0f, moveSpeed * dt, 0.0f));
      }

      if (Input::GetKeyHold(KeyCode::KEY_LEFT))
      {
        g_camera.Rotate(glm::vec3(0.0f, -rotateSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_RIGHT))
      {
        g_camera.Rotate(glm::vec3(0.0f, rotateSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_UP))
      {
        g_camera.Rotate(glm::vec3(rotateSpeed * dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_DOWN))
      {
        g_camera.Rotate(glm::vec3(-rotateSpeed * dt, 0.0f, 0.0f));
      }

      glm::vec2 offset = Input::GetMouseOffset();
      float pitch = -offset.y * mouseSpeed * dt;
      float yaw = offset.x * mouseSpeed * dt;
      g_camera.Rotate(glm::vec3(pitch
        , yaw, 0.0f));
    }
  }

  void ApplyLight(Shader& shader)
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

  void DrawLoop(Shader& shader)
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
    Debug::Log << "Rendering::Initialize\n";

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
    int width = Window::GetWidth(), height = Window::GetHeight();

    //Depth FBO for shadow
    m_initResolution.x = width, m_initResolution.y = height;
    m_shadowWidth = width, m_shadowHeight = height;

    m_depthfbo.Init();
    m_shadowMapTexture = m_depthfbo.AttachDepthTexture(m_shadowWidth, m_shadowHeight);

    m_depthMaterial.InitShader("Shadow/depth_directional.vert"
      , "Shadow/depth_directional.frag");

    //Depth FBO for point shadow
    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
    {
      m_depth2fbo[i].Init();

      m_shadowMapCubemap[i].InitDepthCubemap(m_shadowWidth);
      m_depth2fbo[i].AttachCubemap(m_shadowMapCubemap[i]);
    }

    m_depth2Material.InitShader("Shadow/depth_point.vert"
      , "Shadow/depth_point.frag", "Shadow/depth_point.geom");

    //Scene Texture
    m_sceneTexture = Texture::GenerateNullTexture(width, height
      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
      , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
    m_sceneRbo.Init(width, height);

    //Scene Frame Buffer
    m_gbuffer.Init(width, height);
    m_sceneFbo.Init();
    m_sceneFbo.AttachTexture(m_sceneTexture);
    m_sceneFbo.AttachRenderBuffer(m_sceneRbo);
    m_sceneFbo.Bind();

    //************************************************
    // Postprocess
    //************************************************
    m_postProcessSetting = &(SceneManager::GetPostProcessSetting());
    m_postProcessSetting->Init(width, height);

    //Screen Quad
    m_screenQuadVAO.Init();
    m_screenQuadVAO.Build(BufferMode::Static, Quad::texturedVertices
      , Quad::indices, Quad::texturedInfo);

    m_screenTriangleVAO.Init();
    m_screenTriangleVAO.Build(BufferMode::Static, Triangle::vertices
      , Triangle::indices, Triangle::info);
    
    m_postfxFinalMaterial.InitShader("Utility/fullscreenTriangle.vert"
      , "Postprocess/finaldraw.frag");
    m_blitCopyMaterial.InitShader("Utility/fullscreenTriangle.vert"
      , "Utility/blitCopy.frag");

    //************************************************
    // Cubemap
    //************************************************
    //IBL
    m_ibl.Init(g_camera, m_screenQuadVAO);

    //************************************************
    // Material
    //************************************************
    m_defaultMaterial = &(SceneManager::GetDefaultMaterial());
    m_defaultMaterial->InitShader("Rendering/deferred_geometry_pass.vert"
      , "Rendering/deferred_geometry_pass.frag");

    m_defaultMaterial->InitTexture(FileSystem::GetFilePath("diffuse_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , true, FileSystem::GetFilePath("normal_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("emissive_wood.png", FileSystem::DirectoryType::Textures));

    m_lightingMaterial.InitShader("Rendering/deferred_lighting_pbr_pass.vert"
      , "Rendering/deferred_lighting_pbr_pass.frag");

    m_lightingMaterial.Bind(false);
    {
      Shader& shader = m_lightingMaterial.GetShader();

      //Directional Shadow
      shader.SetUniform("u_shadowMap2D", 6);
      m_shadowMapTexture.BindToTextureUnit(6);

      //Point Shadow
      shader.SetUniform("u_shadowMap[0]", 7);
      shader.SetUniform("u_shadowMap[1]", 8);
      shader.SetUniform("u_shadowMap[2]", 9);
      shader.SetUniform("u_shadowMap[3]", 10);

      //IBL
      shader.SetUniform("u_irradianceMap", 11);
      shader.SetUniform("u_prefilterMap", 12);
      shader.SetUniform("u_brdfLUT", 13);

      //Gbuffer's texture
      shader.SetUniform("u_gbufferResult.m_positionTex"
        , 0);
      shader.SetUniform("u_gbufferResult.m_normalTex"
        , 1);
      shader.SetUniform("u_gbufferResult.m_albedoTex"
        , 2);
      shader.SetUniform("u_gbufferResult.m_specularTex"
        , 3);
      shader.SetUniform("u_gbufferResult.m_emissiveTex"
        , 4);
      shader.SetUniform("u_gbufferResult.m_lightSpacePos"
        , 5);
    }
    m_lightingMaterial.Unbind();

    //Normal Debugger
    m_normalDebug.InitShader("Debugger/debug_vertex.vert"
      , "Debugger/debug_fragment.frag", "Debugger/debug_normal_geometry.geom");

    m_normalDebug.Bind(false);
    m_normalDebug.GetShader().SetUniform("u_color", glm::vec3(1.0f));
    m_normalDebug.Unbind();

    //Debug material
    m_billboardMaterial = &(SceneManager::GetBillBoardMaterial());
    m_billboardMaterial->InitShader("Debugger/debug_billboard.vert"
      , "Debugger/debug_fragment.frag");
    m_billboardMaterial->Bind(false);
    m_billboardMaterial->GetShader().SetUniform("u_color", glm::vec3(1.0f));
    m_billboardMaterial->Unbind();

    m_lightTexture = Texture(FileSystem::GetFilePath("Icon_Light.png", FileSystem::DirectoryType::Textures)
      , Texture::Channel::RGBA);

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
    m_defaultMaterial->GetShader().SetUniformBlockBindingPoint("u_matrices", 0);
    m_uniformBufferObject.Init(sizeof(glm::mat4) * 2, 0);
    m_uniformBufferObject.FillBuffer(sizeof(glm::mat4), sizeof(glm::mat4)
      , glm::value_ptr(g_camera.GetProjectionMatrix()));

    //************************************************
    // Init Projection Matrix
    //************************************************
    //Set Projection Matrix once
    //g_camera.ApplyProjectionMatrix(g_defaultMaterial.GetShader());
    g_camera.ApplyProjectionMatrix(m_billboardMaterial->GetShader());
    g_camera.ApplyProjectionMatrix(m_normalDebug.GetShader());
  }

  void RenderLoopOpengl::Terminate(void)
  {
    Debug::Log << "Rendering::Terminate\n";

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

    InstanceDrawer::UnregisterAllInstances();

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
    ProcessInput(dt);

    //*************************************************
    // Rendering Loop
    //*************************************************
    if (!Window::ShouldClose())
    {
#if(EDITOR_MODE)
      Editor::PreRender();
      Render();
      Editor::PostRender();
#else
      Render();
#endif

      // Flip Buffers and Draw
      Window::SwapBuffer();
      glfwPollEvents();
    }
  }

  void RenderLoopOpengl::OnRecompiledShader(void)
  {
    //Geometry Pass
    m_defaultMaterial->RefreshTextureUniforms();
    ResourceManager::RefreshMaterialTextureUniforms();

    //Lighting Pass
    m_lightingMaterial.Bind(false);
    {
      Shader& shader = m_lightingMaterial.GetShader();

      //Directional Shadow
      shader.SetUniform("u_shadowMap2D", 6);
      m_shadowMapTexture.BindToTextureUnit(6);

      //Point Shadow
      shader.SetUniform("u_shadowMap[0]", 7);
      shader.SetUniform("u_shadowMap[1]", 8);
      shader.SetUniform("u_shadowMap[2]", 9);
      shader.SetUniform("u_shadowMap[3]", 10);

      //IBL
      shader.SetUniform("u_irradianceMap", 11);
      shader.SetUniform("u_prefilterMap", 12);
      shader.SetUniform("u_brdfLUT", 13);

      //Gbuffer's texture
      shader.SetUniform("u_gbufferResult.m_positionTex"
        , 0);
      shader.SetUniform("u_gbufferResult.m_normalTex"
        , 1);
      shader.SetUniform("u_gbufferResult.m_albedoTex"
        , 2);
      shader.SetUniform("u_gbufferResult.m_specularTex"
        , 3);
      shader.SetUniform("u_gbufferResult.m_emissiveTex"
        , 4);
      shader.SetUniform("u_gbufferResult.m_lightSpacePos"
        , 5);
    }
    m_lightingMaterial.Unbind();
    
    //IBL
    m_ibl.RefreshTextureUniforms(g_camera);

    //Postprocessing
    m_postProcessSetting->RefreshTextureUniforms();
  }

  /////////////////////////////////////////////////////////////

  void RenderLoopOpengl::Render(void)
  {
    //Clear BG first
    const float clear_color = 0.0f;
    glClearColor(clear_color, clear_color
      , clear_color, clear_color);

    //*************************************************
    // Depth FBO Pass for directional light shadow
    //*************************************************
    glViewport(0, 0, m_shadowWidth, m_shadowHeight);
    glEnable(GL_DEPTH_TEST);

    //TODO: don't refresh lights component every frame
    SceneManager::GetLights(g_sceneLights);

    //Shader and Matrices
    glm::mat4 lightSpaceMatrix;
    if (g_sceneLights.dirLights.size() > 0)
    {
      auto lightComponent = g_sceneLights.dirLights[0]->GetComponent("Light");
      lightSpaceMatrix = lightComponent->Get<Light>()
        ->CalculateLightSpaceMatrix(g_camera, 10.0f, 0.01f, 100.0f);

      //Draw pass to FBO
      m_depthfbo.Bind();
      {
        glClear(GL_DEPTH_BUFFER_BIT);
        m_depthMaterial.Bind(false);
        {
          m_depthMaterial.GetShader().SetUniform("u_lightSpaceMatrix"
            , lightSpaceMatrix);

          //Draw all Mesh with depthMaterial
          Drawer::DrawShadowWithoutBind(m_depthMaterial.GetShader()
            , Drawer::DrawPass::BATCH);
          Drawer::DrawShadowWithoutBind(m_depthMaterial.GetShader()
            , Drawer::DrawPass::CUSTOM);
        }
        m_depthMaterial.Unbind();
      }
      m_depthfbo.Unbind();
    }

    //*************************************************
    // Depth FBO Pass for point shadow
    //*************************************************
    glViewport(0, 0, m_shadowWidth, m_shadowWidth);
    const float farPlane = g_camera.m_far;
    if (g_sceneLights.pointLights.size() > 0)
    {
      for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
      {
        //Shader and Matrices
        auto pointLightComponent = g_sceneLights.pointLights[i]->GetComponent("Light");
        auto& lightSpaceMatrices = pointLightComponent->Get<Light>()
          ->CalculateLightSpaceMatrices(90.0f, 1.0f, 0.01f, farPlane);

        //Draw to FBO
        m_depth2fbo[i].Bind();
        {
          glClear(GL_DEPTH_BUFFER_BIT);
          //Depth Material
          m_depth2Material.Bind(false);
          {
            for (int i = 0; i < 6; ++i)
            {
              m_depth2Material.GetShader().SetUniform(g_lightSpaceMatrices[i]
                , lightSpaceMatrices[i]);
            }
            m_depth2Material.GetShader().SetUniform("u_lightPos"
              , g_sceneLights.pointLights[i]->GetTransform()->GetPosition());
            m_depth2Material.GetShader().SetUniform("u_farPlane"
              , farPlane);
            //Draw all Mesh with depthMaterial
            Drawer::DrawShadowWithoutBind(m_depth2Material.GetShader());
          }
          m_depth2Material.Unbind();
        }
        m_depth2fbo[i].Unbind();
      }
    }

    //*************************************************
    // Geometry Pass
    //*************************************************
    glViewport(0, 0, m_initResolution.x, m_initResolution.y);
    m_gbuffer.Bind();
    {
      //Clear Buffer
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT);

      //Set Uniform
      m_defaultMaterial->Bind();
      {
        Shader& shader = m_defaultMaterial->GetShader();
        shader.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
      }
      m_defaultMaterial->Unbind();

      //Draw Scene
      DrawScene(false);
    }
    m_gbuffer.Unbind();


    //*************************************************
    // Lighting Pass
    //*************************************************
    m_gbuffer.CopyDepthBufferTo(m_sceneFbo.GetID());
    m_sceneFbo.Bind();
    {
      //Clear Buffer
      glClear(GL_COLOR_BUFFER_BIT);

      //Set Material Uniform
      m_lightingMaterial.Bind(false);
      {
        Shader& shader = m_lightingMaterial.GetShader();
        shader.SetUniform("u_farPlane", farPlane);

        //Shadow 2D texture
        m_shadowMapTexture.BindToTextureUnit(6);

        //Shadow Cubemap texture
        for (int j = 0; j < POINTLIGHT_AMOUNT; ++j)
        {
          m_shadowMapCubemap[j].BindToTextureUnit(7 + j);
        }

        //IBL
        m_ibl.m_irradianceCubemap.BindToTextureUnit(11);
        m_ibl.m_prefilterMap.BindToTextureUnit(12);
        m_ibl.m_brdfLUT.BindToTextureUnit(13);

        //Bind Gbuffer Texture
        m_gbuffer.BindTextures();

        //Camera Position
        g_camera.ApplyCameraInfo(shader);

        //Apply Light information to the Shader
        ApplyLight(shader);

        //Draw Quad
        m_screenQuadVAO.Draw();
      }
      m_lightingMaterial.Unbind();

      //Draw Cubemap
      m_ibl.DrawCubemap(IBL::CubemapType::SKYBOX, g_camera);

    }
    m_sceneFbo.Unbind();

    //*************************************************
    // PostProcess Pass
    //*************************************************
    //FXAA
    static bool enablePostprocess = true;
    if (Input::GetKeyDown(Input::KeyCode::KEY_8))
    {
      enablePostprocess = !enablePostprocess;
      if (!enablePostprocess)
      {
        m_postProcessSetting->Clear();
      }
    }
    if (enablePostprocess)
    {
      m_postProcessSetting->Apply(PostProcessContext{ &g_camera, &m_gbuffer
        , &m_screenQuadVAO, &m_sceneTexture });
    }

    //*************************************************
    // Final Pass to the screen
    //*************************************************
    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    //Debugging View
    static size_t g_bufferIndex = 0;
    static bool g_debugDeferred = false;
    static bool g_showLight = true;
    if (Input::GetKeyDown(Input::KeyCode::KEY_0))
    {
      g_bufferIndex = (g_bufferIndex + 1) %
        static_cast<unsigned>(GBufferTarget::Count);
    }
    if (Input::GetKeyDown(Input::KeyCode::KEY_9))
    {
      g_debugDeferred = !g_debugDeferred;
    }
    if (Input::GetKeyDown(Input::KeyCode::KEY_7))
    {
      g_showLight = !g_showLight;
    }

    //Draw Screen
    m_sceneFbo.Bind();
    {
      m_postfxFinalMaterial.Bind(false);
      {
        Shader& shader = m_postfxFinalMaterial.GetShader();
        shader.SetUniform("u_screenTexture", 0);
        shader.SetUniform("u_bloomTexture", 1);
        shader.SetUniform("u_ssaoTexture", 2);
        shader.SetUniform("u_exposure", 1.0f);

        //Scene Texture
        if (g_debugDeferred)
        {
          shader.SetUniform("u_bloomTexture", 0);
          m_gbuffer.GetTexture(g_bufferIndex).BindToTextureUnit(0);
        }
        else
        {
          m_sceneTexture.BindToTextureUnit(0);
          m_postProcessSetting->m_bloomPP.m_targetTexture.BindToTextureUnit(1);
          m_postProcessSetting->m_ssaoPP.m_ssaoTexture.BindToTextureUnit(2);
        }

        //g_shadowMapTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
        m_screenTriangleVAO.Draw();
      }
      m_postfxFinalMaterial.Unbind();

      //*************************************************
      // Forward Rendering afterward
      //*************************************************
      if (g_showLight)
      {
        glEnable(GL_DEPTH_TEST);
        Texture::SetBlendMode(true);
        //Draw Light Icons Billboard
        g_camera.ApplyViewMatrix(m_billboardMaterial->GetShader());
        m_billboardMaterial->Bind(false);
        {
          Shader& shader = m_billboardMaterial->GetShader();
          shader.SetUniform("u_texture", 0);
          m_lightTexture.BindToTextureUnit(0);

          Drawer::DrawWithoutBind(shader
            , Drawer::DrawPass::DEBUG);
        }
        m_billboardMaterial->Unbind();
        Texture::SetBlendMode(false);
      }

      //*************************************************
      // Physics Debug Draw
      //*************************************************
      glClear(GL_DEPTH_BUFFER_BIT);
      auto physicsScene = Physics::PhysicsScene::GetPhysicsScene(0);
      if (physicsScene != nullptr)
      {
        physicsScene->DebugDraw(g_camera);
      }
    }
    m_sceneFbo.Unbind();

    //*************************************************
    // FXAA at the end, Directly onto the screen
    //*************************************************
    if (enablePostprocess && m_postProcessSetting->m_fxaaPP.m_enable)
    {
      m_postProcessSetting->m_fxaaPP.ApplyToScreen(m_screenQuadVAO
        , m_sceneTexture);
    }
    else
    {
      m_blitCopyMaterial.Bind(false);
      {
        m_blitCopyMaterial.GetShader().SetUniform("u_screenTexture", 0);
        m_sceneTexture.BindToTextureUnit(0);

        //Draw Quad
        m_screenTriangleVAO.Draw();
      }
      m_blitCopyMaterial.Unbind();
    }
  }

  void RenderLoopOpengl::DrawScene(bool debugNormal)
  {
    //Update View matrix to Shader
    m_uniformBufferObject.FillBuffer(0, sizeof(glm::mat4)
      , glm::value_ptr(g_camera.GetViewMatix()));

    //g_camera.ApplyViewMatrix(g_defaultMaterial.GetShader());
    g_camera.ApplyViewMatrix(m_normalDebug.GetShader());

    //Bind Shader
    m_defaultMaterial->Bind();
    {
      Shader& shader = m_defaultMaterial->GetShader();

      //Draw Static Instances
      InstanceDrawer::DrawInstances(shader);

      //Draw Loop by traversing Containers
      Drawer::DrawWithoutBind(shader, Drawer::DrawPass::BATCH);

      //Draw Custom Pass
      Drawer::Draw(Drawer::DrawPass::CUSTOM);
    }
    m_defaultMaterial->Unbind();

    //For debugging normal
    if (debugNormal)
    {
      m_normalDebug.Bind(false);
      {
        Drawer::DrawWithoutBind(m_normalDebug.GetShader());
      }
      m_normalDebug.Unbind();
    }
  }
} // Rendering
