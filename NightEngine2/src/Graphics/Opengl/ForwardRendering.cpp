/*!
  @file ForwardRendering.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ForwardRendering
*/
//#include "Rendering/Window.hpp"
//#include "Rendering/Editor/Editor.hpp"
//
////FrameBuffer Test
//#include "Rendering/FrameBufferObject.hpp"
//#include "Rendering/RenderBufferObject.hpp"
//
//#include "Rendering/PrimitiveShape.hpp"
//#include "Rendering/InstanceDrawer.hpp"
//
////TODO: Remove these later
//#include "Rendering/CameraObject.hpp"
//#include "Rendering/Material.hpp"
//#include "Rendering/VertexArrayObject.hpp"
//#include "Rendering/Vertex.hpp"
//#include "Rendering/Model.hpp"
//#include "Rendering/Light.hpp"
//#include "Rendering/Cubemap.hpp"
//#include "Rendering/UniformBufferObject.hpp"
//
//#include "NightEngine/EC/Components/MeshRenderer.hpp"
//#include "NightEngine/EC/Factory.hpp"
//#include "NightEngine/EC/GameObject.hpp"
//#include "NightEngine/EC/Handle.hpp"
//
////Subsystem
//#include "NightEngine/Logger.hpp"
//#include "NightEngine/Serialization/FileSystem.hpp"
//#include "Input/Input.hpp"
//
//// System Headers
//#include <assimp/Importer.hpp>
//#include <assimp/postprocess.h>
//#include <assimp/scene.h>
//#include <btBulletDynamicsCommon.h>
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <assimp/anim.h>
//
//#define EDITOR_MODE true
//#define POINTLIGHT_AMOUNT 4
//#define SPOTLIGHT_AMOUNT 4
//
//using namespace NightEngine;
//using namespace NightEngine::EC;
//using namespace NightEngine::EC::Components;
//using namespace Rendering::PrimitiveShape;
//using namespace NightEngine::Factory;
//
//namespace Rendering
//{
//  //Uniform Buffer Object
//  UniformBufferObject g_ubo;
//
//  //Depth FrameBuffer for Directional Shadow
//  FrameBufferObject   g_depthfbo;
//  Texture             g_shadowMapTexture;
//  Material            g_depthMaterial;
//
//  //Depth FrameBuffer for Point Shadow
//  FrameBufferObject   g_depth2fbo[POINTLIGHT_AMOUNT];
//  Cubemap             g_shadowMapCubemap[POINTLIGHT_AMOUNT];
//  Material            g_depth2Material;
//  const std::string   g_lightSpaceMatrices[]{
//     "u_lightSpaceMatrices[0]","u_lightSpaceMatrices[1]"
//    ,"u_lightSpaceMatrices[2]","u_lightSpaceMatrices[3]"
//    ,"u_lightSpaceMatrices[4]" ,"u_lightSpaceMatrices[5]" };
//
//  float               g_initWidth, g_initHeight;
//
//  //Scene FrameBuffer
//  FrameBufferObject   g_sceneFbo;
//  Texture             g_sceneTexture;
//  RenderBufferObject  g_sceneRbo;
//
//  //Bloom FBO
//  FrameBufferObject   g_bloomFbo;
//  Texture             g_bloomTexture;
//  Material            g_thresholdMaterial;
//  Material            g_blurMaterial;
//
//  //Screen Quad
//  VertexArrayObject   g_screenVAO;
//  Material            g_screenMaterial;
//
//  //Cubemap
//  Cubemap             g_cubemap;
//
//  //Material
//  Material     g_defaultMaterial;
//  Material     g_normalDebug;
//
//  //Model
//  std::vector<Handle<GameObject>>   g_boxInstances;
//
//  Handle<GameObject>   g_boxGO;
//  Handle<GameObject>   g_floorGO;
//  Handle<GameObject>   g_modelGO1;
//  Handle<GameObject>   g_modelGO2;
//
//  //Camera
//  CameraObject g_cam{CameraObject::CameraType::PERSPECTIVE
//    ,103.0f};
//
//  //Light
//  Handle<GameObject>   g_dirLight;
//  Handle<GameObject>   g_pointLight[POINTLIGHT_AMOUNT];
//  Handle<GameObject>   g_spotLight[SPOTLIGHT_AMOUNT];
//  Material     g_debugMaterial;
//  Material     g_dirLightMaterial;
//
//	void Initialize(void)
//	{
//		Debug::Log << "Rendering::Initialize\n";
//
//		//Init window
//		Window::Initialize("NightEngine", Window::WindowMode::WINDOW);
//
//    //************************************************
//    // Opengl Setting
//    //************************************************
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL); //So Cubemap drawn with DepthValue == 1.0
//
//    //Set stencil value if it is 0
//    glEnable(GL_STENCIL_TEST);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    glStencilFunc(GL_ALWAYS, 1, 0xFF);
//    glStencilMask(0x00);  //Disable write
//
//    //Face culling
//    glEnable(GL_CULL_FACE);
//
//#if(EDITOR_MODE)
//		Editor::Initialize();
//#endif
//
//    //************************************************
//    // Frame Buffer Object
//    //************************************************
//     float width = Window::GetWidth(), height = Window::GetHeight();
//
//    //Depth FBO for shadow
//    g_initWidth = width, g_initHeight = height;
//    g_depthfbo.Init();
//    g_shadowMapTexture = g_depthfbo.AttachDepthTexture(g_initWidth, g_initHeight);
//
//    g_depthMaterial.InitShader("Shadow/depth_directional.vert"
//      , "Shadow/depth_directional.frag");
//
//    //Depth FBO for point shadow
//    for (int i = 0; i < POINTLIGHT_AMOUNT; ++i)
//    {
//      g_depth2fbo[i].Init();
//
//      g_shadowMapCubemap[i].InitDepthCubemap(g_initWidth);
//      g_depth2fbo[i].AttachCubemap(g_shadowMapCubemap[i]);
//    }
//
//    g_depth2Material.InitShader("Shadow/depth_point.vert"
//      , "Shadow/depth_point.frag", "Shadow/depth_point.geom");
//
//    //Scene Texture
//    g_sceneTexture = Texture::GenerateNullTexture(width, height
//      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
//      , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
//    g_sceneRbo.Init(width, height);
//
//    //Scene Frame Buffer
//    g_sceneFbo.Init();
//    g_sceneFbo.AttachTexture(g_sceneTexture);
//    g_sceneFbo.AttachRenderBuffer(g_sceneRbo);
//    g_sceneFbo.Bind();
//
//    //Bloom Frame Buffer
//    g_bloomTexture = Texture::GenerateNullTexture(width, height
//      , Texture::Channel::RGBA16F, Texture::Channel::RGBA
//      , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);
//    g_bloomFbo.Init();
//    g_bloomFbo.AttachTexture(g_bloomTexture);
//    g_bloomFbo.Bind();
//
//    g_thresholdMaterial.InitShader("Postprocess/brightness_threshold.vert"
//      , "Postprocess/brightness_threshold.frag");
//    g_blurMaterial.InitShader("Postprocess/gaussian_blur.vert"
//      , "Postprocess/gaussian_blur.frag");
//
//    //Screen Quad
//    g_screenVAO.Init();
//    g_screenVAO.Build(BufferMode::Static, Quad::texturedVertices
//      , Quad::indices, Quad::texturedInfo);
//    g_screenMaterial.InitShader("Postprocess/finaldraw.vert"
//      , "Postprocess/finaldraw.frag");
//
//    //************************************************
//    // Cubemap
//    //************************************************
//    std::vector<std::string> cubemapFiles = Cubemap::GetCubemapFileNames("hw_lagoon");
//    g_cubemap.Init(cubemapFiles,"skybox.vert"
//      , "skybox.frag", Texture::Channel::SRGB);
//
//    g_cam.ApplyProjectionMatrix(g_cubemap.GetShader());
//
//    //************************************************
//    // Material
//    //************************************************
//    g_defaultMaterial.InitShader("Lighting/blinnphong_forward.vert"
//      , "Lighting/blinnphong_forward.frag");
//
//    g_defaultMaterial.InitPBRTexture(FileSystem::GetFilePath("diffuse_brickwall.jpg", FileSystem::DirectoryType::Textures)
//      , true ,FileSystem::GetFilePath("normal_brickwall.jpg", FileSystem::DirectoryType::Textures)
//      , FileSystem::GetFilePath("specular_wood.png", FileSystem::DirectoryType::Textures)
//      , FileSystem::GetFilePath("emissive_wood.png", FileSystem::DirectoryType::Textures));
//
//    g_defaultMaterial.Bind(false);
//    {
//      g_defaultMaterial.GetShader().SetUniform("u_shadowMap2D", 4);
//      g_shadowMapTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_4);
//
//      g_defaultMaterial.GetShader().SetUniform("u_shadowMap[0]", 5);
//      g_defaultMaterial.GetShader().SetUniform("u_shadowMap[1]", 6);
//      g_defaultMaterial.GetShader().SetUniform("u_shadowMap[2]", 7);
//      g_defaultMaterial.GetShader().SetUniform("u_shadowMap[3]", 8);
//    }
//    g_defaultMaterial.Unbind();
//
//    //Normal Debugger
//    g_normalDebug.InitShader("Debug/debug_vertex.vert"
//      , "Debug/debug_fragment.frag", "Debug/debug_normal_geometry.geom");
//
//    g_normalDebug.Bind(false);
//    g_normalDebug.GetShader().SetUniform("u_color", glm::vec3(1.0f));
//    g_normalDebug.Unbind();
//
//    //Debug material
//    g_debugMaterial.InitShader("Debug/debug_vertex.vert"
//      , "Debug/debug_fragment.frag");
//    g_debugMaterial.Bind(false);
//    g_debugMaterial.GetShader().SetUniform("u_color", glm::vec3(1.0f));
//    g_debugMaterial.Unbind();
//
//    //DirLight material
//    g_dirLightMaterial.InitShader("Debug/debug_vertex.vert"
//      , "Debug/debug_fragment.frag");
//    g_dirLightMaterial.Bind(false);
//    g_dirLightMaterial.GetShader().SetUniform("u_color", glm::vec3(0.7f, 0.2f, 0.0f));
//    g_dirLightMaterial.Unbind();
//
//    //************************************************
//    // Setup Box, Model GameObjects
//    //************************************************
//    std::vector<glm::mat4> modelMat;
//    for (int i = 0; i < 10; i++)
//    {
//      modelMat.push_back( 
//        Transform::CalculateModelMatrix(glm::vec3(i, 10.0f, 4.0f)
//        , glm::quat(), glm::vec3(1.0f)));
//    }
//
//    //Box instances
//    g_boxInstances.reserve(1000);
//    int y = 0, x = 0;
//    for (int i = 0; i < 1000; ++i)
//    {
//      std::string name = "BoxInstances" + std::to_string(i);
//      g_boxInstances.emplace_back(GameObject::Create(name.c_str(), 2));
//      GameObject& g = *g_boxInstances.back().Get();
//      g.AddComponent("MeshRenderer");
//      auto mr = g.GetComponent("MeshRenderer");
//      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
//        , FileSystem::DirectoryType::Models), false);
//      
//      if (i %2 == 0)
//      {
//        mr->Get<MeshRenderer>()->SetMaterial(&g_dirLightMaterial);
//      }
//      else
//      {
//        mr->Get<MeshRenderer>()->SetMaterial(&g_debugMaterial);
//      }
//      mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::STATIC);
//
//      g.GetTransform()->SetPosition(glm::vec3(x, y, -10.0f));
//      g.GetTransform()->SetScale(glm::vec3(1.0f));
//      g.GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, -0.5f));
//
//      //Arrange position collumn/row
//      ++x;
//      if (x >= 50)
//      {
//        x = 0;
//        ++y;
//      }
//    }
//
//    //Model1
//    g_modelGO1 = GameObject::Create("Model1", 1);
//    g_modelGO1->AddComponent("MeshRenderer");
//    auto c = g_modelGO1->GetComponent("MeshRenderer");
//    c->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Torus.obj"
//      , FileSystem::DirectoryType::Models), true);
//    c->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::NORMAL);
//    g_modelGO1->GetTransform()->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
//
//    //Model2
//    g_modelGO2 = GameObject::Create("Model2", 1);
//    /*g_modelGO2.AddComponent("MeshRenderer");
//    mr = g_modelGO2.GetComponent("MeshRenderer");
//    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Nanosuit/nanosuit.obj"
//      , FileSystem::DirectoryType::Models), true);
//    mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::NORMAL);
//    g_modelGO2.GetTransform()->SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
//    g_modelGO2.GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));*/
//
//    //Dirlight
//    g_dirLight = GameObject::Create("Dirlight", 2);
//    g_dirLight->AddComponent("CharacterInfo");
//    g_dirLight->AddComponent("MeshRenderer");
//    auto mr = g_dirLight->GetComponent("MeshRenderer");
//    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
//      , FileSystem::DirectoryType::Models), true);
//    mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::OUTLINE);
//    mr->Get<MeshRenderer>()->SetMaterial(&g_dirLightMaterial);
//
//    g_dirLight->GetTransform()->SetPosition(glm::vec3(0.0f, 15.0f, 2.0f));
//    g_dirLight->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
//    g_dirLight->GetTransform()->SetEulerAngle(glm::vec3(2.0f, 0.0f, 0.0f));
//
//    g_dirLight->AddComponent("Light");
//    mr = g_dirLight->GetComponent("Light");
//    mr->Get<Light>()->Init(Light::LightType::DIRECTIONAL
//      , { glm::vec3(1.0f)
//      ,{ Light::LightInfo::Value{ 0.5f } } }, 0);
//
//    //Spotlight
//    for (size_t i = 0; i < SPOTLIGHT_AMOUNT; ++i)
//    {
//      //Point light
//      std::string name{ "Pointlight" };
//      name += std::to_string(i);
//      g_pointLight[i] = GameObject::Create(name.c_str(), 2);
//      g_pointLight[i]->AddComponent("MeshRenderer");
//      mr = g_pointLight[i]->GetComponent("MeshRenderer");
//      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
//        , FileSystem::DirectoryType::Models), true);
//      mr->Get<MeshRenderer>()->SetMaterial(&g_debugMaterial);
//      mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::OUTLINE);
//
//      g_pointLight[i]->GetTransform()->SetPosition(glm::vec3((float)(i)-1.0f, -1.0f, 0.25f));
//      g_pointLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
//      g_pointLight[i]->GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, 0.0f));
//
//      g_pointLight[i]->AddComponent("Light");
//      mr = g_pointLight[i]->GetComponent("Light");
//      mr->Get<Light>()->Init(Light::LightType::POINT
//        , { glm::vec3(0.0f, 1.0f,0.0f)
//        ,{ Light::LightInfo::Value{ 2.0f } } }, i);
//
//      //Spotlight
//      name = std::string{ "Spotlight" };
//      name += std::to_string(i);
//      g_spotLight[i] = GameObject::Create(name.c_str(), 2);
//      g_spotLight[i]->AddComponent("MeshRenderer");
//      mr = g_spotLight[i]->GetComponent("MeshRenderer");
//      mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
//        , FileSystem::DirectoryType::Models), true);
//      mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::OUTLINE);
//      mr->Get<MeshRenderer>()->SetMaterial(&g_debugMaterial);
//
//      g_spotLight[i]->GetTransform()->SetPosition(glm::vec3((float)i, 5.0f, 2.0f));
//      g_spotLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
//      g_spotLight[i]->GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, -0.5f));
//
//      g_spotLight[i]->AddComponent("Light");
//      mr = g_spotLight[i]->GetComponent("Light");
//      mr->Get<Light>()->Init(Light::LightType::SPOTLIGHT
//        , { glm::vec3(0.0f,0.0f,i / float(POINTLIGHT_AMOUNT))
//        , { Light::LightInfo::Value{ 45.0f, 55.0f, 5.0f } } }, i);
//    }
//
//    //Box
//    g_boxGO = GameObject::Create("Box", 1);
//    g_boxGO->AddComponent("MeshRenderer");
//    mr = g_boxGO->GetComponent("MeshRenderer");
//    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
//      , FileSystem::DirectoryType::Models), true);
//    mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::NORMAL);
//    g_boxGO->GetTransform()->SetPosition(glm::vec3(2.0f, -2.6f, 0.0f));
//
//    //Floor
//    g_floorGO = GameObject::Create("Floor", 1);
//    g_floorGO->AddComponent("MeshRenderer");
//    mr = g_floorGO->GetComponent("MeshRenderer");
//    mr->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
//      , FileSystem::DirectoryType::Models), true);
//    mr->Get<MeshRenderer>()->SetDrawMode(MeshRenderer::DrawMode::NORMAL);
//    g_floorGO->GetTransform()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
//    g_floorGO->GetTransform()->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
//
//    //************************************************
//    // Uniform Buffer Object
//    //************************************************
//    g_defaultMaterial.GetShader().SetUniformBlockBindingPoint("u_matrices", 0);
//    g_ubo.Init(sizeof(glm::mat4) * 2, 0);
//    g_ubo.FillBuffer(sizeof(glm::mat4), sizeof(glm::mat4)
//      , glm::value_ptr(g_cam.GetProjectionMatrix()));
//
//    //************************************************
//    // Init Projection Matrix
//    //************************************************
//    //Set Projection Matrix once
//    //g_cam.ApplyProjectionMatrix(g_defaultMaterial.GetShader());
//    g_cam.ApplyProjectionMatrix(g_debugMaterial.GetShader());
//    g_cam.ApplyProjectionMatrix(g_dirLightMaterial.GetShader());
//
//    g_cam.ApplyProjectionMatrix(g_normalDebug.GetShader());
//
//    //************************************************
//    // Build InstanceDrawer
//    //************************************************
//    InstanceDrawer::BuildAllDrawer();
//	}
//
//	void ProcessInput(float dt)
//	{
//		using namespace Input;
//		static float moveSpeed = 4.0f;
//    static const float walkSpeed = 4.0f;
//    static const float runSpeed = 10.0f;
//
//		static float mouseSpeed = 10.0f;
//		static float rotateSpeed = 50.0f;
//
//		if (Input::GetMouseHold(MouseKeyCode::MOUSE_BUTTON_RIGHT))
//		{
//      if (Input::GetKeyHold(KeyCode::KEY_LEFT_SHIFT))
//      {
//        moveSpeed = runSpeed;
//      }
//      else
//      {
//        moveSpeed = walkSpeed;
//      }
//			if (Input::GetKeyHold(KeyCode::KEY_W))
//			{
//				g_cam.Move(glm::vec3(0.0f, 0.0f, moveSpeed *dt));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_S))
//			{
//				g_cam.Move(glm::vec3(0.0f, 0.0f, -moveSpeed *dt));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_D))
//			{
//				g_cam.Move(glm::vec3(moveSpeed*dt, 0.0f, 0.0f));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_A))
//			{
//				g_cam.Move(glm::vec3(-moveSpeed*dt, 0.0f, 0.0f));
//			}
//      if (Input::GetKeyHold(KeyCode::KEY_Q))
//      {
//        g_cam.Move(glm::vec3(0.0f, -moveSpeed * dt, 0.0f));
//      }
//      if (Input::GetKeyHold(KeyCode::KEY_E))
//      {
//        g_cam.Move(glm::vec3(0.0f, moveSpeed * dt, 0.0f));
//      }
//
//			if (Input::GetKeyHold(KeyCode::KEY_LEFT))
//			{
//				g_cam.Rotate(glm::vec3(0.0f, -rotateSpeed * dt, 0.0f));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_RIGHT))
//			{
//				g_cam.Rotate(glm::vec3(0.0f, rotateSpeed * dt, 0.0f));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_UP))
//			{
//				g_cam.Rotate(glm::vec3(rotateSpeed * dt, 0.0f, 0.0f));
//			}
//			if (Input::GetKeyHold(KeyCode::KEY_DOWN))
//			{
//				g_cam.Rotate(glm::vec3(-rotateSpeed * dt, 0.0f, 0.0f));
//			}
//
//			glm::vec2 offset = Input::GetMouseOffset();
//			float pitch = -offset.y * mouseSpeed * dt;
//			float yaw = offset.x * mouseSpeed * dt;
//			g_cam.Rotate(glm::vec3(pitch
//				, yaw, 0.0f));
//		}
//	}
//
//  void ApplyLight(Shader& shader)
//  {
//    //Light Apply loop
//    auto& lightContainer = Factory::GetTypeContainer<Light>();
//    {
//      auto it = lightContainer.GetIterator();
//      while (!it.IsEnd())
//      {
//        auto light = it.Get();
//        light->ApplyLightInfo(shader);
//
//        it.Next();
//      }
//    }
//  }
//
//  void DrawLoop(Shader& shader)
//  {
//    //MeshRenderer Draw Loop
//    auto& meshRendererContainer = Factory::GetTypeContainer<MeshRenderer>();
//    auto it = meshRendererContainer.GetIterator();
//    while (!it.IsEnd())
//    {
//      auto mr = it.Get();
//      mr->DrawWithoutBind(false, shader);
//
//      it.Next();
//    }
//  }
//
//  void DrawScene(bool debugNormal)
//  {
//    //Update View matrix to Shader
//    g_ubo.FillBuffer(0, sizeof(glm::mat4)
//      , glm::value_ptr(g_cam.GetViewMatix()));
//
//    //g_cam.ApplyViewMatrix(g_defaultMaterial.GetShader());
//    g_cam.ApplyViewMatrix(g_debugMaterial.GetShader());
//    g_cam.ApplyViewMatrix(g_dirLightMaterial.GetShader());
//    g_cam.ApplyViewMatrix(g_normalDebug.GetShader());
//
//    //Bind Shader
//    g_defaultMaterial.Bind();
//    {
//      Shader& shader = g_defaultMaterial.GetShader();
//      float time = (float)glfwGetTime();
//      //g_defaultMaterial.GetShader().SetUniform("u_time", time);
//      g_cam.ApplyCameraInfo(shader);
//
//      //Apply Light information to the Shader
//      ApplyLight(shader);
//
//      //Draw Instances
//      InstanceDrawer::DrawInstances(shader);
//
//      //Draw Loop by traversing Containers
//      Drawer::DrawWithoutBind(shader);
//    }
//    g_defaultMaterial.Unbind();
//
//    //For debugging normal
//    if (debugNormal)
//    {
//      g_normalDebug.Bind(false);
//      {
//        Drawer::DrawWithoutBind(g_normalDebug.GetShader());
//      }
//      g_normalDebug.Unbind();
//    }
//  }
//
//	void Render(float dt)
//	{
//		ProcessInput(dt);
//
//    //*************************************************
//		// Rendering Loop
//    //*************************************************
//		if (!Window::ShouldClose())
//		{
//#if(EDITOR_MODE)
//      //*************************************************
//      // Rendering 
//      //*************************************************
//
//      //*************************************************
//      // Depth FBO Pass for shadow
//      //*************************************************
//      glViewport(0, 0, g_initWidth, g_initHeight);
//      glEnable(GL_DEPTH_TEST);
//      //Shader and Matrices
//      auto lightComponent = g_dirLight->GetComponent("Light");
//      auto lightSpaceMatrix = lightComponent->Get<Light>()
//        ->CalculateDirLightWorldToLightSpaceMatrix(g_cam, 25.0f, 0.01f, 100.0f);
//
//      //Draw pass to FBO
//      g_depthfbo.Bind();
//      {
//        glClear(GL_DEPTH_BUFFER_BIT);
//        g_depthMaterial.Bind(false);
//        {
//          g_depthMaterial.GetShader().SetUniform("u_lightSpaceMatrix"
//            , lightSpaceMatrix);
//
//          //Draw all Mesh with depthMaterial
//          Drawer::DrawShadowWithoutBind(g_depthMaterial.GetShader());
//        }
//        g_depthMaterial.Unbind();
//      }
//      g_depthfbo.Unbind();
//
//      //*************************************************
//      // Depth FBO Pass for point shadow
//      //*************************************************
//      glViewport(0, 0, g_initWidth, g_initWidth);
//      const float farPlane = g_cam.m_far;
//      for(int i =0;i < POINTLIGHT_AMOUNT; ++i)
//      {
//        //Shader and Matrices
//        auto pointLightComponent = g_pointLight[i]->GetComponent("Light");
//        auto lightSpaceMatrices = pointLightComponent->Get<Light>()
//          ->CalculatePointLightWorldToLightSpaceMatrices(90.0f, 1.0f, 0.01f, farPlane);
//
//        //Draw to FBO
//        g_depth2fbo[i].Bind();
//        {
//          glClear(GL_DEPTH_BUFFER_BIT);
//          //Depth Material
//          g_depth2Material.Bind(false);
//          {
//            for (int i = 0; i < 6; ++i)
//            {
//              //TODO: don't concatenate string every frame
//              g_depth2Material.GetShader().SetUniform(g_lightSpaceMatrices[i]
//                , lightSpaceMatrices[i]);
//            }
//            g_depth2Material.GetShader().SetUniform("u_lightPos"
//              , g_pointLight[i]->GetTransform()->GetPosition());
//            g_depth2Material.GetShader().SetUniform("u_farPlane"
//              , farPlane);
//            //Draw all Mesh with depthMaterial
//            Drawer::DrawShadowWithoutBind(g_depth2Material.GetShader());
//          }
//          g_depth2Material.Unbind();
//        }
//        g_depth2fbo[i].Unbind();
//      }
//
//      //*************************************************
//      // Scene pass to the FBO Texture
//      //*************************************************
//      glViewport(0, 0, g_initWidth, g_initHeight);
//      g_sceneFbo.Bind();
//      {
//        Editor::PreRender();
//
//        //Clear Buffer
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
//          | GL_STENCIL_BUFFER_BIT);
//        glEnable(GL_DEPTH_TEST);
//
//        //Set Material Uniform
//        g_defaultMaterial.Bind(false);
//        {
//          g_defaultMaterial.GetShader().SetUniform("u_lightSpaceMatrix"
//            , lightSpaceMatrix);
//          g_defaultMaterial.GetShader().SetUniform("u_farPlane"
//            , farPlane);
//
//          //Shadow 2D texture
//          g_shadowMapTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_4);
//          
//          //Shadow Cubemap texture
//          for (int j = 0; j < POINTLIGHT_AMOUNT; ++j)
//          {
//            glActiveTexture(GL_TEXTURE5 + j);
//            g_shadowMapCubemap[j].Bind();
//          }
//        }
//        g_defaultMaterial.Unbind();
//
//        //Draw Scene
//        DrawScene(false);
//
//        //Draw Cubemap
//        g_cam.ApplyViewMatrix(g_cubemap);
//        g_cubemap.Draw();
//      }
//      g_sceneFbo.Unbind();
//
//      //*************************************************
//      // Bloom pass
//      //*************************************************
//      g_bloomFbo.Bind();
//      {
//        glDisable(GL_DEPTH_TEST);
//
//        //Get the brightness threshold
//        g_thresholdMaterial.Bind(false);
//        {
//          Shader& shader = g_thresholdMaterial.GetShader();
//          shader.SetUniform("u_threshold", 1.0f);
//          shader.SetUniform("u_screenTexture", 0);
//          g_sceneTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
//
//          g_screenVAO.Draw();
//        }
//        g_thresholdMaterial.Unbind();
//      }
//      g_bloomFbo.Unbind();
//
//      //Blur the texture
//      const int blurIteration = 15;
//      for (int i = 0; i < blurIteration; ++i)
//      {
//        g_bloomFbo.Bind();
//        {
//          g_blurMaterial.Bind(false);
//          {
//            Shader& shader = g_blurMaterial.GetShader();
//            glm::vec2 dir(i % 2, (i + 1) % 2);
//            shader.SetUniform("u_dir", dir);
//
//            shader.SetUniform("u_screenTexture", 0);
//            g_bloomTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
//
//            g_screenVAO.Draw();
//          }
//          g_blurMaterial.Unbind(); 
//        }
//        g_bloomFbo.Unbind();
//      }
//
//      //*************************************************
//      // Final Pass to the screen
//      //*************************************************
//      glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//      glClearColor(1.0f, 1.0f,1.0f,1.0f);
//      glClear(GL_COLOR_BUFFER_BIT);
//      glDisable(GL_DEPTH_TEST);
//
//      g_screenMaterial.Bind(false);
//      {
//        Shader& shader = g_screenMaterial.GetShader();
//        shader.SetUniform("u_screenTexture", 0);
//        shader.SetUniform("u_bloomTexture", 1);
//        shader.SetUniform("u_exposure", 0.5f);
//
//        g_sceneTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
//        g_bloomTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_1);
//        //g_shadowMapTexture.BindToTextureUnit(Texture::TextureUnit::TEXTURE_0);
//        g_screenVAO.Draw();
//      }
//      g_screenMaterial.Unbind();
//  
//      //*************************************************
//      // End Rendering 
//      //*************************************************
//
//			Editor::PostRender();
//#else
//			// Background Fill Color
//			glClearColor(val, 0.25f, 0.25f, 1.0f);
//			glClear(GL_COLOR_BUFFER_BIT);
//#endif
//
//			// Flip Buffers and Draw
//			Window::SwapBuffer();
//			glfwPollEvents();
//		}
//	}
//
//	void Terminate(void)
//	{
//		Debug::Log << "Rendering::Terminate\n";
//
//#if(EDITOR_MODE)
//		Editor::Terminate();
//#endif
//
//		Window::Terminate();
//		glfwTerminate();
//	}
//
//} // Rendering
