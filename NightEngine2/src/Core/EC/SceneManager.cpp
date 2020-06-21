/*!
  @file SceneManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of SceneManager
*/

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/Scene.hpp"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Graphics/Opengl/Light.hpp"

#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/InstanceDrawer.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"

#include <btBulletCollisionCommon.h>
#include "Physics/PhysicsScene.hpp"
#include "Physics/Collider.hpp"

//Serialization
#include "Core/Serialization/FileSystem.hpp"
#include "Core/Serialization/ResourceManager.hpp"
#include "Core/Serialization/Serialization.hpp"

#include "Core/Utility/Utility.hpp"

#define POINTLIGHT_AMOUNT 4
#define SPOTLIGHT_AMOUNT 4

using namespace Rendering;
using namespace NightEngine::EC::Components;

namespace NightEngine
{
  namespace EC
  {
    namespace SceneManager
    {
      static Container::Vector<Handle<Scene>> g_openedScenes;
      static Handle<Scene> g_activeScene;

      //PostProcess
      Postprocess::PostProcessSetting* g_postProcessSetting;

      // TODO: care about this material's life time
      // right now its destructor is being called after deleted the whole engine
      static Handle<Material>          g_defaultMaterial;
      static Handle<Material>          g_billboardMaterial;
      static Handle<Material>          g_errorMaterial;

      FACTORY_FUNC_IMPLEMENTATION(Scene);

      void Initialize(void)
      {
        Debug::Log << "SceneManager::Initialize\n";

        FACTORY_REGISTER_TYPE_WITHPARAM(Scene, 1, 5);

        if (!g_defaultMaterial.IsValid())
        {
          g_defaultMaterial = Factory::Create<Material>("Material");
        }
        if (!g_billboardMaterial.IsValid())
        {
          g_billboardMaterial = Factory::Create<Material>("Material");
        }

        //auto scene = LoadScene("Default_Scene");
        //auto scene = CreateDefaultScene("Default_Scene");
        auto scene = CreateEmptyScene("Empty_Scene");
      }

      void Update(void)
      {
        //Debug::Log << "SceneManager::Update\n";
      }

      void FixedUpdate(void)
      {
        //Debug::Log << "SceneManager::FixedUpdate\n";
      }

      void Terminate(void)
      {
        Debug::Log << "SceneManager::Terminate\n";
        g_openedScenes.clear();

        g_defaultMaterial.m_handle.Nullify();
        g_billboardMaterial.m_handle.Nullify();
        g_errorMaterial.m_handle.Nullify();
      }

      /////////////////////////////////////////

      void AddScene(Handle<Scene> scene)
      {
        g_openedScenes.emplace_back(scene);

        // The only avaliable scene will be the active scene
        if (g_openedScenes.size() == 1)
        {
          g_activeScene = g_openedScenes[0];
        }
      }

      void InitLoadedScene(Scene & scene)
      {
        auto& gameObjects = scene.GetAllGameObjects();
        for (auto go : gameObjects)
        {
          auto gameObject = go.Get();

          //Init MeshRenderer
          auto handle = gameObject->GetComponent<MeshRenderer>();
          if (handle != nullptr)
          {
            auto meshRenderer = handle->Get<MeshRenderer>();

            //Set Default Material if null
            auto& defaultMaterial = SceneManager::GetDefaultMaterial();
            if (meshRenderer->GetMaterial() == nullptr)
            {
              meshRenderer->SetMaterial(defaultMaterial);
            }

            //Init Mesh
            auto drawMode = meshRenderer->GetDrawMode();
            meshRenderer->LoadModel(meshRenderer->GetMeshLoadPath()
              , drawMode == MeshRenderer::DrawMode::STATIC ? false : true);
            meshRenderer->RegisterDrawMode(drawMode);
          }

          //Init Light
          handle = gameObject->GetComponent("Light");
          if (handle != nullptr)
          {
            auto light = handle->Get<Light>();
            light->Init(light->GetLightType(), light->GetLightInfo()
              , light->GetLightIndex());
          }

          //Init Rigidbody
          handle = gameObject->GetComponent("Rigidbody");
          if (handle != nullptr)
          {
            auto rigid = handle->Get<Rigidbody>();
            rigid->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
              , gameObject->GetTransform()->GetPosition()
              , rigid->GetColliderInitializer());
          }
        }

        //************************************************
        // Build InstanceDrawer
        //************************************************
        Rendering::GPUInstancedDrawer::BuildAllDrawer();
      }

      Handle<Scene> CreateEmptyScene(Container::String sceneName)
      {
        auto sceneHandle = Factory::Create<Scene>("Scene");
        Scene& scene = *(sceneHandle.Get());

        GetUniqueName(sceneName);
        scene.SetSceneName(sceneName);

        ComponentHandle* ch;
        Handle<GameObject>   floorGO;

        Handle<GameObject>   dirLight;
        //************************************************
        // Preloading Models
        //  TODO: Serialize a list of mesh object into scene file for preloading
        //************************************************
        {
          std::vector<std::string> filePaths{
            FileSystem::GetFilePath("Quad.obj", FileSystem::DirectoryType::Models) };
          ResourceManager::PreloadModelsResourceAsync(filePaths);
        }

        //Models
        {
          //Floor
          floorGO = GameObject::Create("Floor", 1);
          floorGO->AddComponent("MeshRenderer");
          ch = floorGO->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::PREBIND);
          g_defaultMaterial.Get()->SetParams(0.2f, 0.1f);
          ch->Get<MeshRenderer>()->SetMaterial(g_defaultMaterial);
          floorGO->GetTransform()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
          floorGO->GetTransform()->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
          floorGO->AddComponent("Rigidbody");
          ch = floorGO->GetComponent("Rigidbody");
          ch->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , floorGO->GetTransform()->GetPosition()
            , Physics::ColliderInitializer{ 
              Physics::ColliderType::BOX_COLLIDER
              , glm::vec3(10.0f, 1.0f, 10.0f) } );

          scene.AddGameObject(floorGO);
        }

        //Lights
        {
          //Dirlight
          dirLight = GameObject::Create("Dirlight", 2);
          dirLight->AddComponent("CharacterInfo");
          dirLight->AddComponent("MeshRenderer");
          ch = dirLight->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);
          ch->Get<MeshRenderer>()->SetMaterial(g_billboardMaterial);

          dirLight->GetTransform()->SetPosition(glm::vec3(0.0f, 15.0f, 2.0f));
          dirLight->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
          dirLight->GetTransform()->SetEulerAngle(glm::vec3(2.0f, 0.0f, 0.0f));

          dirLight->AddComponent("Light");
          ch = dirLight->GetComponent("Light");
          ch->Get<Light>()->Init(Light::LightType::DIRECTIONAL
            , { glm::vec3(1.0f)
            ,{ Light::LightInfo::Value{ 0.5f } } }, 0);
        }

        //Add to Scene
        scene.AddGameObject(dirLight);

        AddScene(sceneHandle);
        return sceneHandle;
      }

      Handle<Scene> CreateDefaultScene(Container::String sceneName)
      {
        auto sceneHandle = Factory::Create<Scene>("Scene");
        Scene& scene = *(sceneHandle.Get());
        scene.SetSceneName(sceneName);

        ComponentHandle* ch;

        std::vector<Handle<GameObject>>   boxInstances;
        Handle<GameObject>   sphereGO;
        Handle<GameObject>   floorGO;
        Handle<GameObject>   modelGO1;
        Handle<GameObject>   modelGO2;

        Handle<GameObject>   dirLight;
        Handle<GameObject>   pointLight[POINTLIGHT_AMOUNT];
        Handle<GameObject>   spotLight[SPOTLIGHT_AMOUNT];
        //************************************************
        // Preloading Models
        //  TODO: Serialize a list of mesh object into scene file for preloading
        //************************************************
        {
          std::vector<std::string> filePaths{
            FileSystem::GetFilePath("Cube.obj", FileSystem::DirectoryType::Models),
            FileSystem::GetFilePath("Torus.obj", FileSystem::DirectoryType::Models),
            FileSystem::GetFilePath("guts-berserker/guts.fbx", FileSystem::DirectoryType::Models),
            FileSystem::GetFilePath("Quad.obj", FileSystem::DirectoryType::Models),
            FileSystem::GetFilePath("Sphere.obj", FileSystem::DirectoryType::Models) };
          ResourceManager::PreloadModelsResourceAsync(filePaths);
        }

        //Models
        {
          //Model1
          modelGO1 = GameObject::Create("Model1", 1);
          modelGO1->AddComponent("MeshRenderer");
          ch = modelGO1->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Torus.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::PREBIND);
          modelGO1->GetTransform()->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
          modelGO1->AddComponent("Rigidbody");
          ch = modelGO1->GetComponent("Rigidbody");
          ch->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , modelGO1->GetTransform()->GetPosition()
            , Physics::ColliderInitializer{ Physics::ColliderType::BOX_COLLIDER, glm::vec3(1.0f, 0.6f, 1.0f) }
            , 1.0f);

          //Model2
          modelGO2 = GameObject::Create("Model2", 1);
          modelGO2->AddComponent("MeshRenderer");
          ch = modelGO2->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("guts-berserker/guts.fbx"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::CUSTOM);
          ch->Get<MeshRenderer>()->LoadMaterial("Guts.mat");
          modelGO2->GetTransform()->SetPosition(glm::vec3(2.0f, 10.0f, 0.0f));
          modelGO2->GetTransform()->SetScale(glm::vec3(4.5f, 4.5f, 4.5f));
          modelGO2->AddComponent("Rigidbody");
          ch = modelGO2->GetComponent("Rigidbody");
          ch->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , modelGO2->GetTransform()->GetPosition()
            , Physics::ColliderInitializer{ Physics::ColliderType::CYLINDER_COLLIDER, glm::vec3(1.0f) }
            , 1.0f);

          //Sphere
          sphereGO = GameObject::Create("Sphere", 1);
          sphereGO->AddComponent("MeshRenderer");
          ch = sphereGO->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::PREBIND);
          sphereGO->GetTransform()->SetPosition(glm::vec3(2.0f, -2.6f, 0.0f));
          sphereGO->AddComponent("Rigidbody");
          ch = sphereGO->GetComponent("Rigidbody");
          ch->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , glm::vec3(0.0f, 50.0f, 0.0f)//sphereGO->GetTransform()->GetPosition()
            , Physics::ColliderInitializer{ Physics::ColliderType::SPHERE_COLLIDER, glm::vec3(1.0f) }
            , 1.0f);

          //Floor
          floorGO = GameObject::Create("Floor", 1);
          floorGO->AddComponent("MeshRenderer");
          ch = floorGO->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::PREBIND);
          g_defaultMaterial.Get()->SetParams(0.2f, 0.1f);
          ch->Get<MeshRenderer>()->SetMaterial(g_defaultMaterial);
          floorGO->GetTransform()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
          floorGO->GetTransform()->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
          floorGO->AddComponent("Rigidbody");
          ch = floorGO->GetComponent("Rigidbody");
          ch->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , floorGO->GetTransform()->GetPosition()
            , Physics::ColliderInitializer{ 
              Physics::ColliderType::BOX_COLLIDER
              , glm::vec3(10.0f, 1.0f, 10.0f) });

          //Add to Scene
          scene.AddGameObject(sphereGO);
          scene.AddGameObject(floorGO);
          scene.AddGameObject(modelGO1);
          scene.AddGameObject(modelGO2);
        }

        //Box instances
        {
          const int k_size = 1000;
          boxInstances.reserve(k_size);
          int y = 0, x = 0;
          for (int i = 0; i < k_size; ++i)
          {
            std::string name = "BoxInstances" + std::to_string(i);
            boxInstances.emplace_back(GameObject::Create(name.c_str(), 2));
            GameObject& g = *boxInstances.back().Get();
            g.AddComponent("MeshRenderer");
            ch = g.GetComponent("MeshRenderer");
            ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
              , FileSystem::DirectoryType::Models), false);

            if (i % 2 == 0)
            {
              ch->Get<MeshRenderer>()->SetMaterial(g_defaultMaterial);
            }
            else
            {
              ch->Get<MeshRenderer>()->SetMaterial(g_defaultMaterial);
            }
            ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::STATIC);

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

            //Add to Scene
            scene.AddGameObject(boxInstances[i]);
          }

          //************************************************
          // Build InstanceDrawer
          //************************************************
          GPUInstancedDrawer::BuildAllDrawer();
        }

        //Lights
        {
          //Dirlight
          dirLight = GameObject::Create("Dirlight", 2);
          dirLight->AddComponent("CharacterInfo");
          dirLight->AddComponent("MeshRenderer");
          ch = dirLight->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);
          ch->Get<MeshRenderer>()->SetMaterial(g_billboardMaterial);

          dirLight->GetTransform()->SetPosition(glm::vec3(0.0f, 15.0f, 2.0f));
          dirLight->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
          dirLight->GetTransform()->SetEulerAngle(glm::vec3(2.0f, 0.0f, 0.0f));

          dirLight->AddComponent("Light");
          ch = dirLight->GetComponent("Light");
          ch->Get<Light>()->Init(Light::LightType::DIRECTIONAL
            , { glm::vec3(1.0f)
            ,{ Light::LightInfo::Value{ 0.5f } } }, 0);

          //Spotlight
          for (size_t i = 0; i < SPOTLIGHT_AMOUNT; ++i)
          {
            //Point light
            std::string name{ "Pointlight" };
            name += std::to_string(i);
            pointLight[i] = GameObject::Create(name.c_str(), 2);
            pointLight[i]->AddComponent("MeshRenderer");
            ch = pointLight[i]->GetComponent("MeshRenderer");
            ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
              , FileSystem::DirectoryType::Models), true, false);
            ch->Get<MeshRenderer>()->SetMaterial(g_billboardMaterial);
            ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);

            pointLight[i]->GetTransform()->SetPosition(glm::vec3((float)(i * 4.0f) - 6.0f, -1.0f, 0.25f));
            pointLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
            pointLight[i]->GetTransform()->SetEulerAngle(glm::vec3(0.0f, 0.0f, 0.0f));

            pointLight[i]->AddComponent("Light");
            ch = pointLight[i]->GetComponent("Light");
            ch->Get<Light>()->Init(Light::LightType::POINT
              , { glm::vec3(0.0f, 1.0f,0.0f)
              ,{ Light::LightInfo::Value{ 8.0f } } }, i);

            //Spotlight
            name = std::string{ "Spotlight" };
            name += std::to_string(i);
            spotLight[i] = GameObject::Create(name.c_str(), 2);
            spotLight[i]->AddComponent("MeshRenderer");
            ch = spotLight[i]->GetComponent("MeshRenderer");
            ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Quad.obj"
              , FileSystem::DirectoryType::Models), true);
            ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::DEBUG);
            ch->Get<MeshRenderer>()->SetMaterial(g_billboardMaterial);

            spotLight[i]->GetTransform()->SetPosition(glm::vec3((float)i* 4.0f - 6.0f, 5.0f, 2.0f));
            spotLight[i]->GetTransform()->SetScale(glm::vec3(0.2f, 0.2f, 0.2f));
            spotLight[i]->GetTransform()->SetEulerAngle(glm::vec3(4.5f, 0.0f, 0.0f));

            spotLight[i]->AddComponent("Light");
            ch = spotLight[i]->GetComponent("Light");
            ch->Get<Light>()->Init(Light::LightType::SPOTLIGHT
              ,
              { glm::vec3(0.0f,0.0f, 0.7f)
                , Light::LightInfo::Value{ 1.0f, 0.95f, 5.0f } }, i);

            //Add to Scene
            scene.AddGameObject(pointLight[i]);
            scene.AddGameObject(spotLight[i]);
          }
        }

        //Add to Scene
        scene.AddGameObject(dirLight);

        AddScene(sceneHandle);
        return sceneHandle;
      }

      /////////////////////////////////////////

      Handle<Scene> LoadScene(Container::String sceneName)
      {
        std::string fileName{ sceneName };
        fileName += ".nscene";

        Scene scene;
        Serialization::DeserializeFromFile(scene
          , fileName
          , FileSystem::DirectoryType::Scenes
          , SceneManager::DeserializeScene);

        Debug::Log << Logger::MessageType::INFO
          << "Load Scene:" << fileName << '\n';

        //Create and copy from the deserialized scene
        auto handle = Factory::Create<Scene>("Scene");
        *(handle.Get()) = scene;
        InitLoadedScene(scene);

        AddScene(handle);
        return handle;
      }

      void CloseScene(Handle<Scene> scene)
      {
        //Remove from openning list
        for (auto it = g_openedScenes.begin();
          it != g_openedScenes.end(); ++it)
        {
          if (scene.m_handle == it->m_handle)
          {
            g_openedScenes.erase(it);
            break;
          }
        }

        //Remove all gameObject in the scene then destroy the scene object itself
        auto gameObjects = scene->GetAllGameObjects();
        for (int i = 0; i < gameObjects.size(); ++i)
        {
          gameObjects[i]->Destroy();
        }
        scene->Clear();
        scene.Destroy();
      }

      void SaveScene(Handle<Scene> scene)
      {
        std::string fileName{ scene->GetSceneName() };
        SaveSceneAs(scene, fileName);
      }

      void SaveSceneAs(Handle<Scene> scene, std::string fileName)
      {
        NightEngine::Utility::StopWatch stopWatch{ true };
        {
          Scene& sceneObj = *(scene.Get());
          sceneObj.SetSceneName(fileName);
          fileName += ".nscene";

          Serialization::SerializeToFile(sceneObj
            , fileName
            , FileSystem::DirectoryType::Scenes
            , SceneManager::SerializeScene);
        }
        stopWatch.Stop();

        Debug::Log << Logger::MessageType::INFO
          << "Saved Scene:" << fileName << " [" << stopWatch.GetElapsedTimeMilli() << " ms]\n";
      }

      void SaveAllScenes(void)
      {
        Debug::Log << "SceneManager::SaveAllScenes\n";
        for (int i = 0; i < g_openedScenes.size(); ++i)
        {
          SaveScene(g_openedScenes[i]);
        }
      }

      void RemoveGameObjectFromScene(Handle<GameObject> gameObject)
      {
        for (int i = 0; i < g_openedScenes.size(); ++i)
        {
          g_openedScenes[i]->RemoveGameObject(gameObject);
        }
      }

      void ReregisterAllMeshRenderer()
      {
        Debug::Log << "SceneManager::ReregisterAllMeshRenderer\n";
        for (int i = 0; i < g_openedScenes.size(); ++i)
        {
          auto& gameObjects = g_openedScenes[i].Get()->GetAllGameObjects();
          for (auto go : gameObjects)
          {
            auto gameObject = go.Get();

            //Init MeshRenderer
            auto handle = gameObject->GetComponent<MeshRenderer>();
            if (handle != nullptr)
            {
              auto meshRenderer = handle->Get<MeshRenderer>();
              meshRenderer->ReregisterDrawMode();
            }
          }
        }
      }

      /////////////////////////////////////////

      Container::Vector<Handle<Scene>>* GetAllScenes(void)
      {
        return &g_openedScenes;
      }

      Handle<Scene> GetActiveScene(void)
      {
        return g_activeScene;
      }

      void SetActiveScene(Handle<Scene> scene)
      {
        g_activeScene = scene;
      }

      /////////////////////////////////////////

      void GetUniqueName(Container::String& sceneName)
      {
        for (int i = 0; i < g_openedScenes.size(); ++i)
        {
          if (g_openedScenes[i]->GetSceneName() == sceneName)
          {
            sceneName += "_";
          }
        }
      }

      bool GetLights(SceneLights& sceneLights)
      {
        sceneLights.Clear();

        bool found = false;
        for (int i = 0; i < g_openedScenes.size(); ++i)
        {
          //Traverse all gameobjects in the scene
          auto& gameObjects = g_openedScenes[i]->GetAllGameObjects();
          for (auto& gameObjectHandle : gameObjects)
          {
            auto lightComponent = gameObjectHandle.Get()->GetComponent("Light");
            if (lightComponent != nullptr)
            {
              auto light = lightComponent->Get<Light>();

              switch (light->GetLightType())
              {
              case Light::LightType::DIRECTIONAL:
                sceneLights.dirLights.emplace_back(gameObjectHandle);
                break;

              case Light::LightType::POINT:
                sceneLights.pointLights.emplace_back(gameObjectHandle);
                break;

              case Light::LightType::SPOTLIGHT:
                sceneLights.spotLights.emplace_back(gameObjectHandle);
                break;
              }
              found = true;
            }
          }
        }

        return found;
      }

      Handle<Rendering::Material> GetDefaultMaterial(void)
      {
        if (!g_defaultMaterial.IsValid())
        {
          g_defaultMaterial = Factory::Create<Material>("Material");
        }
        return g_defaultMaterial;
      }

      Handle<Rendering::Material> GetBillBoardMaterial(void)
      {
        if (!g_billboardMaterial.IsValid())
        {
          g_billboardMaterial = Factory::Create<Material>("Material");
        }
        return g_billboardMaterial;
      }

      Handle<Rendering::Material> GetErrorMaterial(void)
      {
        if (!g_errorMaterial.IsValid())
        {
          g_errorMaterial = Material::LoadMaterial("ErrorMaterial.mat");
        }
        return g_errorMaterial;
      }

      /////////////////////////////////////////

      Rendering::Postprocess::PostProcessSetting& GetPostProcessSetting(void)
      {
        if (g_postProcessSetting == nullptr)
        {
          g_postProcessSetting = new Rendering::Postprocess::PostProcessSetting();
        }

        return *g_postProcessSetting;
      }

      void DeletePostProcessSetting(void)
      {
        if (g_postProcessSetting != nullptr)
        {
          delete g_postProcessSetting;
          g_postProcessSetting = nullptr;
        }
      }
    }
  }
}