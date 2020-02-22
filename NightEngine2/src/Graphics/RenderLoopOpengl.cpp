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

//GameObject
#include "Core/EC/Components/MeshRenderer.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Factory.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"

//Subsystem
#include "Core/Serialization/ResourceManager.hpp"
#include "Core/Logger.hpp"
#include "Core/Serialization/FileSystem.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Input/Input.hpp"

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

//TODO: Remove Later
#include <btBulletCollisionCommon.h>
#include "Physics/PhysicsScene.hpp"
#include "Physics/Collider.hpp"
#include "RenderLoopOpengl.hpp"

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

  //Model
  std::vector<Handle<GameObject>>   g_boxInstances;

  Handle<GameObject>   g_sphereGO;
  Handle<GameObject>   g_floorGO;
  Handle<GameObject>   g_modelGO1;
  Handle<GameObject>   g_modelGO2;

  //Camera
  CameraObject g_camera{ CameraObject::CameraType::PERSPECTIVE
    ,103.0f };

  //Light
  Handle<GameObject>   g_dirLight;
  Handle<GameObject>   g_pointLight[POINTLIGHT_AMOUNT];
  Handle<GameObject>   g_spotLight[SPOTLIGHT_AMOUNT];

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

  void RenderLoopOpengl::Render(void)
  {
    //Clear BG first
    const float clear_color = 0.0f;
    glClearColor(clear_color, clear_color
      , clear_color, clear_color);

    //*************************************************
    // Depth FBO Pass for shadow
    //*************************************************
    glViewport(0, 0, g_shadowWidth, g_shadowHeight);
    glEnable(GL_DEPTH_TEST);
    //Shader and Matrices
    auto lightComponent = g_dirLight->GetComponent("Light");
    auto& lightSpaceMatrix = lightComponent->Get<Light>()
      ->CalculateLightSpaceMatrix(g_camera, 10.0f, 0.01f, 100.0f);

    //Draw pass to FBO
    g_depthfbo.Bind();
    {
      glClear(GL_DEPTH_BUFFER_BIT);
      g_depthMaterial.Bind(false);
      {
        g_depthMaterial.GetShader().SetUniform("u_lightSpaceMatrix"
          , lightSpaceMatrix);

        //Draw all Mesh with depthMaterial
        Drawer::DrawShadowWithoutBind(g_depthMaterial.GetShader()
          , Drawer::DrawPass::BATCH);
        Drawer::DrawShadowWithoutBind(g_depthMaterial.GetShader()
          , Drawer::DrawPass::CUSTOM);
      }
      g_depthMaterial.Unbind();
    }
    g_depthfbo.Unbind();

    //*************************************************
    // Depth FBO Pass for point shadow
    //*************************************************
    glViewport(0, 0, g_shadowWidth, g_shadowWidth);
    const float farPlane = g_camera.m_far;
    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
    {
      //Shader and Matrices
      auto pointLightComponent = g_pointLight[i]->GetComponent("Light");
      auto& lightSpaceMatrices = pointLightComponent->Get<Light>()
        ->CalculateLightSpaceMatrices(90.0f, 1.0f, 0.01f, farPlane);

      //Draw to FBO
      g_depth2fbo[i].Bind();
      {
        glClear(GL_DEPTH_BUFFER_BIT);
        //Depth Material
        g_depth2Material.Bind(false);
        {
          for (int i = 0; i < 6; ++i)
          {
            g_depth2Material.GetShader().SetUniform(g_lightSpaceMatrices[i]
              , lightSpaceMatrices[i]);
          }
          g_depth2Material.GetShader().SetUniform("u_lightPos"
            , g_pointLight[i]->GetTransform()->GetPosition());
          g_depth2Material.GetShader().SetUniform("u_farPlane"
            , farPlane);
          //Draw all Mesh with depthMaterial
          Drawer::DrawShadowWithoutBind(g_depth2Material.GetShader());
        }
        g_depth2Material.Unbind();
      }
      g_depth2fbo[i].Unbind();
    }

    //*************************************************
    // Geometry Pass
    //*************************************************
    glViewport(0, 0, g_initResolution.x, g_initResolution.y);
    m_gbuffer.Bind();
    {
      //Clear Buffer
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT);

      //Set Uniform
      g_defaultMaterial.Bind();
      {
        Shader& shader = g_defaultMaterial.GetShader();
        shader.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
      }
      g_defaultMaterial.Unbind();

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
      g_lightingMaterial.Bind(false);
      {
        Shader& shader = g_lightingMaterial.GetShader();
        shader.SetUniform("u_farPlane", farPlane);

        //Shadow 2D texture
        g_shadowMapTexture.BindToTextureUnit(6);

        //Shadow Cubemap texture
        for (int j = 0; j < POINTLIGHT_AMOUNT; ++j)
        {
          g_shadowMapCubemap[j].BindToTextureUnit(7 + j);
        }

        //IBL
        g_ibl.m_irradianceCubemap.BindToTextureUnit(11);
        g_ibl.m_prefilterMap.BindToTextureUnit(12);
        g_ibl.m_brdfLUT.BindToTextureUnit(13);

        //Bind Gbuffer Texture
        m_gbuffer.BindTextures();

        //Camera Position
        g_camera.ApplyCameraInfo(shader);

        //Apply Light information to the Shader
        ApplyLight(shader);

        //Draw Quad
        g_screenVAO.Draw();
      }
      g_lightingMaterial.Unbind();

      //Draw Cubemap
      g_ibl.DrawCubemap(IBL::CubemapType::SKYBOX, g_camera);

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
        g_ssaoPP.Clear();
        g_bloomPP.Clear();
      }
    }
    if (enablePostprocess)
    {
      //g_fxaaPP.Apply(g_screenVAO
        //, g_sceneTexture, g_sceneFbo);

      //SSAO
      g_ssaoPP.Apply(g_screenVAO, g_camera, m_gbuffer);

      //Bloom
      g_bloomPP.Apply(g_screenVAO, m_sceneTexture);
    }

    //*************************************************
    // Final Pass to the screen
    //*************************************************
    glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Copy gbuffer depth to the sceneFBO
    m_gbuffer.CopyDepthBufferTo(m_sceneFbo.GetID());
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
      g_screenMaterial.Bind(false);
      {
        Shader& shader = g_screenMaterial.GetShader();
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
          g_bloomPP.m_targetTexture.BindToTextureUnit(1);
          g_ssaoPP.m_ssaoTexture.BindToTextureUnit(2);
        }

        //g_shadowMapTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
        g_screenVAO.Draw();
      }
      g_screenMaterial.Unbind();

      //*************************************************
      // Forward Rendering afterward
      //*************************************************
      if (g_showLight)
      {
        glEnable(GL_DEPTH_TEST);
        Texture::SetBlendMode(true);
        //Draw Light Icons Billboard
        g_camera.ApplyViewMatrix(g_billboardMaterial.GetShader());
        g_billboardMaterial.Bind(false);
        {
          Shader& shader = g_billboardMaterial.GetShader();
          shader.SetUniform("u_texture", 0);
          g_lightTexture.BindToTextureUnit(0);

          Drawer::DrawWithoutBind(shader
            , Drawer::DrawPass::DEBUG);
        }
        g_billboardMaterial.Unbind();
        Texture::SetBlendMode(false);
      }

      //*************************************************
      // Physics Debug Draw
      //*************************************************
      glClear(GL_DEPTH_BUFFER_BIT);
      Physics::PhysicsScene::GetPhysicsScene(0)->DebugDraw(g_camera);
    }
    m_sceneFbo.Unbind();

    //*************************************************
    // FXAA at the end, Directly onto the screen
    //*************************************************
    if (enablePostprocess)
    {
      g_fxaaPP.ApplyToScreen(g_screenVAO
        , m_sceneTexture);
    }
    else
    {
      g_screenQuadMaterial.Bind(false);
      {
        g_screenQuadMaterial.GetShader().SetUniform("u_screenTexture", 0);
        m_sceneTexture.BindToTextureUnit(0);

        //Draw Quad
        g_screenVAO.Draw();
      }
      g_screenQuadMaterial.Unbind();
    }
  }

  void RenderLoopOpengl::DrawScene(bool debugNormal)
  {
    //Update View matrix to Shader
    g_uniformBufferObject.FillBuffer(0, sizeof(glm::mat4)
      , glm::value_ptr(g_camera.GetViewMatix()));

    //g_camera.ApplyViewMatrix(g_defaultMaterial.GetShader());
    g_camera.ApplyViewMatrix(g_normalDebug.GetShader());

    //Bind Shader
    g_defaultMaterial.Bind();
    {
      Shader& shader = g_defaultMaterial.GetShader();

      //Draw Static Instances
      InstanceDrawer::DrawInstances(shader);

      //Draw Loop by traversing Containers
      Drawer::DrawWithoutBind(shader, Drawer::DrawPass::BATCH);

      //Draw Custom Pass
      Drawer::Draw(Drawer::DrawPass::CUSTOM);
    }
    g_defaultMaterial.Unbind();

    //For debugging normal
    if (debugNormal)
    {
      g_normalDebug.Bind(false);
      {
        Drawer::DrawWithoutBind(g_normalDebug.GetShader());
      }
      g_normalDebug.Unbind();
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
    g_initResolution.x = width, g_initResolution.y = height;
    g_shadowWidth = width, g_shadowHeight = height;

    g_depthfbo.Init();
    g_shadowMapTexture = g_depthfbo.AttachDepthTexture(g_shadowWidth, g_shadowHeight);

    g_depthMaterial.InitShader("Shadow/depth_directional.vert"
      , "Shadow/depth_directional.frag");

    //Depth FBO for point shadow
    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
    {
      g_depth2fbo[i].Init();

      g_shadowMapCubemap[i].InitDepthCubemap(g_shadowWidth);
      g_depth2fbo[i].AttachCubemap(g_shadowMapCubemap[i]);
    }

    g_depth2Material.InitShader("Shadow/depth_point.vert"
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
    g_bloomPP.Init(width, height);

    //SSAO
    g_ssaoPP.Init(width, height);

    //FXAA
    g_fxaaPP.Init(width, height);

    //Screen Quad
    g_screenVAO.Init();
    g_screenVAO.Build(BufferMode::Static, Quad::texturedVertices
      , Quad::indices, Quad::texturedInfo);
    g_screenMaterial.InitShader("Postprocess/finaldraw.vert"
      , "Postprocess/finaldraw.frag");
    g_screenQuadMaterial.InitShader("Debugger/debug_textured.vert"
      , "Debugger/debug_textured.frag");

    //************************************************
    // Cubemap
    //************************************************
    //IBL
    g_ibl.Init(g_camera, g_screenVAO);

    //************************************************
    // Material
    //************************************************
    g_defaultMaterial.InitShader("Rendering/deferred_geometry_pass.vert"
      , "Rendering/deferred_geometry_pass.frag");

    g_defaultMaterial.InitTexture(FileSystem::GetFilePath("diffuse_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , true, FileSystem::GetFilePath("normal_brickwall.jpg", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures)
      , FileSystem::GetFilePath("emissive_wood.png", FileSystem::DirectoryType::Textures));

    g_lightingMaterial.InitShader("Rendering/deferred_lighting_pbr_pass.vert"
      , "Rendering/deferred_lighting_pbr_pass.frag");

    g_lightingMaterial.Bind(false);
    {
      Shader& shader = g_lightingMaterial.GetShader();

      //Directional Shadow
      shader.SetUniform("u_shadowMap2D", 6);
      g_shadowMapTexture.BindToTextureUnit(6);

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
    g_lightingMaterial.Unbind();

    //Normal Debugger
    g_normalDebug.InitShader("Debugger/debug_vertex.vert"
      , "Debugger/debug_fragment.frag", "Debugger/debug_normal_geometry.geom");

    g_normalDebug.Bind(false);
    g_normalDebug.GetShader().SetUniform("u_color", glm::vec3(1.0f));
    g_normalDebug.Unbind();

    //Debug material
    g_billboardMaterial.InitShader("Debugger/debug_billboard.vert"
      , "Debugger/debug_fragment.frag");
    g_billboardMaterial.Bind(false);
    g_billboardMaterial.GetShader().SetUniform("u_color", glm::vec3(1.0f));
    g_billboardMaterial.Unbind();

    g_lightTexture = Texture(FileSystem::GetFilePath("Icon_Light.png", FileSystem::DirectoryType::Textures)
      , Texture::Channel::RGBA);

    //************************************************
    // Preloading Models
    //************************************************
    {
      std::vector<std::string> filePaths{ 
        FileSystem::GetFilePath("Cube.obj", FileSystem::DirectoryType::Models), 
        FileSystem::GetFilePath("Torus.obj", FileSystem::DirectoryType::Models),
        FileSystem::GetFilePath("guts-berserker/guts.fbx", FileSystem::DirectoryType::Models),
        FileSystem::GetFilePath("Quad.obj", FileSystem::DirectoryType::Models),
        FileSystem::GetFilePath("Sphere.obj", FileSystem::DirectoryType::Models)};
      ResourceManager::PreloadModelsResourceAsync(filePaths);
    }

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

    //Box instances
    g_boxInstances.reserve(1000);
    int y = 0, x = 0;
    for (int i = 0; i < 1000; ++i)
    {
      std::string name = "BoxInstances" + std::to_string(i);
      g_boxInstances.emplace_back(GameObject::Create(name.c_str(), 2));
      GameObject& g = *g_boxInstances.back().Get();
      g.AddComponent("MeshRenderer");
      auto mr = g.GetComponent("MeshRenderer");
      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
        , FileSystem::DirectoryType::Models), false);

      if (i % 2 == 0)
      {
        mr->Get<MeshRenderer>()->SetMaterial(&g_billboardMaterial);
      }
      else
      {
        mr->Get<MeshRenderer>()->SetMaterial(&g_billboardMaterial);
      }
      mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::STATIC);

      g.GetTransform()->SetPosition(glm::vec3(x, y, -10.0f));
      g.GetTransform()->SetScale(glm::vec3(1.0f));
      g.GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, -0.5f));

      //Arrange position collumn/row
      ++x;
      if (x >= 50)
      {
        x = 0;
        ++y;
      }
    }

    //Model1
    g_modelGO1 = GameObject::Create("Model1", 1);
    g_modelGO1->AddComponent("MeshRenderer");
    auto c = g_modelGO1->GetComponent("MeshRenderer");
    c->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Torus.obj"
      , FileSystem::DirectoryType::Models), true);
    c->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
    g_modelGO1->GetTransform()->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    //g_modelGO1->AddComponent("Rigidbody");
    //c = g_modelGO1->GetComponent("Rigidbody");
    //c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
    //  , g_modelGO1->GetTransform()->GetPosition()
    //  , Physics::BoxCollider(glm::vec3(1.0f, 0.6f, 1.0f)), 1.0f);

    //Model2
    g_modelGO2 = GameObject::Create("Model2", 1);
    g_modelGO2->AddComponent("MeshRenderer");
    c = g_modelGO2->GetComponent("MeshRenderer");
    c->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("guts-berserker/guts.fbx"
      , FileSystem::DirectoryType::Models), true);
    c->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::CUSTOM);
    c->Get<MeshRenderer>()->LoadMaterial("Guts.mat");
    g_modelGO2->GetTransform()->SetPosition(glm::vec3(2.0f, 10.0f, 0.0f));
    g_modelGO2->GetTransform()->SetScale(glm::vec3(4.5f, 4.5f, 4.5f));
    //g_modelGO2->AddComponent("Rigidbody");
    //c = g_modelGO2->GetComponent("Rigidbody");
    //c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
    //  , g_modelGO2->GetTransform()->GetPosition()
    //  , Physics::CylinderCollider(glm::vec3(1.0f))//Physics::CapsuleCollider(1.0f, 2.0f)
    //  , 1.0f);

    //Dirlight
    g_dirLight = GameObject::Create("Dirlight", 2);
    g_dirLight->AddComponent("CharacterInfo");
    g_dirLight->AddComponent("MeshRenderer");
    auto mr = g_dirLight->GetComponent("MeshRenderer");
    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
      , FileSystem::DirectoryType::Models), true);
    mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);
    mr->Get<MeshRenderer>()->SetMaterial(&g_billboardMaterial);

    g_dirLight->GetTransform()->SetPosition(glm::vec3(0.0f, 15.0f, 2.0f));
    g_dirLight->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
    g_dirLight->GetTransform()->SetEulerAngle(glm::vec3(2.0f, 0.0f, 0.0f));

    g_dirLight->AddComponent("Light");
    mr = g_dirLight->GetComponent("Light");
    mr->Get<Light>()->Init(Light::LightType::DIRECTIONAL
      , { glm::vec3(1.0f)
      ,{ Light::LightInfo::Value{ 0.5f } } }, 0);

    //Spotlight
    for (size_t i = 0; i < SPOTLIGHT_AMOUNT; ++i)
    {
      //Point light
      std::string name{ "Pointlight" };
      name += std::to_string(i);
      g_pointLight[i] = GameObject::Create(name.c_str(), 2);
      g_pointLight[i]->AddComponent("MeshRenderer");
      mr = g_pointLight[i]->GetComponent("MeshRenderer");
      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
        , FileSystem::DirectoryType::Models), true, false);
      mr->Get<MeshRenderer>()->SetMaterial(&g_billboardMaterial);
      mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);

      g_pointLight[i]->GetTransform()->SetPosition(glm::vec3((float)(i * 4.0f) - 6.0f, -1.0f, 0.25f));
      g_pointLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
      g_pointLight[i]->GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, 0.0f));

      g_pointLight[i]->AddComponent("Light");
      mr = g_pointLight[i]->GetComponent("Light");
      mr->Get<Light>()->Init(Light::LightType::POINT
        , { glm::vec3(0.0f, 1.0f,0.0f)
        ,{ Light::LightInfo::Value{ 8.0f } } }, i);

      //Spotlight
      name = std::string{ "Spotlight" };
      name += std::to_string(i);
      g_spotLight[i] = GameObject::Create(name.c_str(), 2);
      g_spotLight[i]->AddComponent("MeshRenderer");
      mr = g_spotLight[i]->GetComponent("MeshRenderer");
      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
        , FileSystem::DirectoryType::Models), true);
      mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);
      mr->Get<MeshRenderer>()->SetMaterial(&g_billboardMaterial);

      g_spotLight[i]->GetTransform()->SetPosition(glm::vec3((float)i* 4.0f - 6.0f, 5.0f, 2.0f));
      g_spotLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
      g_spotLight[i]->GetTransform()->SetEulerAngle(glm::vec3(4.5f, 0.0f, 0.0f));

      g_spotLight[i]->AddComponent("Light");
      mr = g_spotLight[i]->GetComponent("Light");
      mr->Get<Light>()->Init(Light::LightType::SPOTLIGHT
        ,
        { glm::vec3(0.0f,0.0f, 0.7f)
          , Light::LightInfo::Value{ 1.0f, 0.95f, 5.0f } }, i);
    }

    //Sphere
    g_sphereGO = GameObject::Create("Sphere", 1);
    g_sphereGO->AddComponent("MeshRenderer");
    mr = g_sphereGO->GetComponent("MeshRenderer");
    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
      , FileSystem::DirectoryType::Models), true);
    mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
    g_sphereGO->GetTransform()->SetPosition(glm::vec3(2.0f, -2.6f, 0.0f));
    //g_sphereGO->AddComponent("Rigidbody");
    //c = g_sphereGO->GetComponent("Rigidbody");
    //c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
    //  , glm::vec3(0.0f, 50.0f, 0.0f)//g_sphereGO->GetTransform()->GetPosition()
    //  , Physics::SphereCollider(1.0f), 1.0f);

    //Floor
    g_floorGO = GameObject::Create("Floor", 1);
    g_floorGO->AddComponent("MeshRenderer");
    mr = g_floorGO->GetComponent("MeshRenderer");
    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
      , FileSystem::DirectoryType::Models), true);
    mr->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
    g_defaultMaterial.SetParams(0.2f, 0.1f);
    mr->Get<MeshRenderer>()->SetMaterial(&g_defaultMaterial);
    g_floorGO->GetTransform()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
    g_floorGO->GetTransform()->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
    //g_floorGO->AddComponent("Rigidbody");
    //c = g_floorGO->GetComponent("Rigidbody");
    //c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
    //  , g_floorGO->GetTransform()->GetPosition()
    //  , Physics::BoxCollider(glm::vec3(10.0f, 1.0f, 10.0f)));

    //************************************************
    // Uniform Buffer Object
    //************************************************
    g_defaultMaterial.GetShader().SetUniformBlockBindingPoint("u_matrices", 0);
    g_uniformBufferObject.Init(sizeof(glm::mat4) * 2, 0);
    g_uniformBufferObject.FillBuffer(sizeof(glm::mat4), sizeof(glm::mat4)
      , glm::value_ptr(g_camera.GetProjectionMatrix()));

    //************************************************
    // Init Projection Matrix
    //************************************************
    //Set Projection Matrix once
    //g_camera.ApplyProjectionMatrix(g_defaultMaterial.GetShader());
    g_camera.ApplyProjectionMatrix(g_billboardMaterial.GetShader());
    g_camera.ApplyProjectionMatrix(g_normalDebug.GetShader());

    //************************************************
    // Build InstanceDrawer
    //************************************************
    InstanceDrawer::BuildAllDrawer();
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

  void RenderLoopOpengl::Terminate(void)
  {
    Debug::Log << "Rendering::Terminate\n";

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

    Window::Terminate();
    OpenglAllocationTracker::PrintAllocationState();
    ShaderTracker::Clear();
  }

  void RenderLoopOpengl::OnRecompiledShader(void)
  {
    //Geometry Pass
    g_defaultMaterial.RefreshTextureUniforms();
    ResourceManager::RefreshMaterialTextureUniforms();

    //Lighting Pass
    g_lightingMaterial.Bind(false);
    {
      Shader& shader = g_lightingMaterial.GetShader();

      //Directional Shadow
      shader.SetUniform("u_shadowMap2D", 6);
      g_shadowMapTexture.BindToTextureUnit(6);

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
    g_lightingMaterial.Unbind();
    
    //IBL
    g_ibl.RefreshTextureUniforms(g_camera);

    //Postprocessing
    g_ssaoPP.RefreshTextureUniforms();
    g_bloomPP.RefreshTextureUniforms();
    g_fxaaPP.RefreshTextureUniforms();
  }
} // Rendering