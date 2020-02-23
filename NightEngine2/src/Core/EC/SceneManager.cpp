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

#include "Core/Serialization/FileSystem.hpp"
#include "Core/Serialization/ResourceManager.hpp"

#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/InstanceDrawer.hpp"

#include <btBulletCollisionCommon.h>
#include "Physics/PhysicsScene.hpp"
#include "Physics/Collider.hpp"

using namespace Rendering;
using namespace NightEngine::EC::Components;

namespace NightEngine
{
  namespace EC
  {
    namespace SceneManager
    {
      static Container::Vector<Handle<Scene>> g_openedScenes;

      //TODO: care about this material's life time
      // right now its destructor is being called after deleted the whole engine
      static Material                g_defaultMaterial;

      FACTORY_FUNC_IMPLEMENTATION(Scene);

      void Initialize(void)
      {
        Debug::Log << "SceneManager::Initialize\n";

        FACTORY_REGISTER_TYPE_WITHPARAM(Scene, 1, 5);

        g_openedScenes.emplace_back(CreateScene());
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

        g_defaultMaterial.Clear();
      }

      /////////////////////////////////////////

      static std::vector<Handle<GameObject>>   g_boxInstances;
      static Handle<GameObject>   g_sphereGO;
      static Handle<GameObject>   g_floorGO;
      static Handle<GameObject>   g_modelGO1;
      static Handle<GameObject>   g_modelGO2;

      Handle<Scene> CreateScene(void)
      {
        auto handle = Factory::Create<Scene>("Scene");

        //************************************************
        // Preloading Models
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

        ComponentHandle* ch;
        {
          Scene& scene = *(handle.Get());

          //Model1
          g_modelGO1 = GameObject::Create("Model1", 1);
          g_modelGO1->AddComponent("MeshRenderer");
          auto c = g_modelGO1->GetComponent("MeshRenderer");
          c->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Torus.obj"
            , FileSystem::DirectoryType::Models), true);
          c->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
          g_modelGO1->GetTransform()->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
          g_modelGO1->AddComponent("Rigidbody");
          c = g_modelGO1->GetComponent("Rigidbody");
          c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , g_modelGO1->GetTransform()->GetPosition()
            , Physics::BoxCollider(glm::vec3(1.0f, 0.6f, 1.0f)), 1.0f);

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
          g_modelGO2->AddComponent("Rigidbody");
          c = g_modelGO2->GetComponent("Rigidbody");
          c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , g_modelGO2->GetTransform()->GetPosition()
            , Physics::CylinderCollider(glm::vec3(1.0f))//Physics::CapsuleCollider(1.0f, 2.0f)
            , 1.0f);

          //Sphere
          g_sphereGO = GameObject::Create("Sphere", 1);
          g_sphereGO->AddComponent("MeshRenderer");
          ch = g_sphereGO->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Sphere.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
          g_sphereGO->GetTransform()->SetPosition(glm::vec3(2.0f, -2.6f, 0.0f));
          g_sphereGO->AddComponent("Rigidbody");
          c = g_sphereGO->GetComponent("Rigidbody");
          c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , glm::vec3(0.0f, 50.0f, 0.0f)//g_sphereGO->GetTransform()->GetPosition()
            , Physics::SphereCollider(1.0f), 1.0f);

          //Floor
          g_floorGO = GameObject::Create("Floor", 1);
          g_floorGO->AddComponent("MeshRenderer");
          ch = g_floorGO->GetComponent("MeshRenderer");
          ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
            , FileSystem::DirectoryType::Models), true);
          ch->Get<MeshRenderer>()->RegisterDrawMode(MeshRenderer::DrawMode::NORMAL);
          g_defaultMaterial.SetParams(0.2f, 0.1f);
          ch->Get<MeshRenderer>()->SetMaterial(&g_defaultMaterial);
          g_floorGO->GetTransform()->SetPosition(glm::vec3(0.0f, -5.0f, 0.0f));
          g_floorGO->GetTransform()->SetScale(glm::vec3(20.0f, 1.0f, 20.0f));
          g_floorGO->AddComponent("Rigidbody");
          c = g_floorGO->GetComponent("Rigidbody");
          c->Get<Rigidbody>()->Initialize(*(Physics::PhysicsScene::GetPhysicsScene(0))
            , g_floorGO->GetTransform()->GetPosition()
            , Physics::BoxCollider(glm::vec3(10.0f, 1.0f, 10.0f)));

          //Box instances
          {
            const int k_size = 1000;
            g_boxInstances.reserve(k_size);
            int y = 0, x = 0;
            for (int i = 0; i < k_size; ++i)
            {
              std::string name = "BoxInstances" + std::to_string(i);
              g_boxInstances.emplace_back(GameObject::Create(name.c_str(), 2));
              GameObject& g = *g_boxInstances.back().Get();
              g.AddComponent("MeshRenderer");
              ch = g.GetComponent("MeshRenderer");
              ch->Get<MeshRenderer>()->LoadModel(FileSystem::GetFilePath("Cube.obj"
                , FileSystem::DirectoryType::Models), false);

              if (i % 2 == 0)
              {
                ch->Get<MeshRenderer>()->SetMaterial(&g_defaultMaterial);
              }
              else
              {
                ch->Get<MeshRenderer>()->SetMaterial(&g_defaultMaterial);
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
            }
          }
          //************************************************
          // Build InstanceDrawer
          //************************************************
          InstanceDrawer::BuildAllDrawer();
        }

        return handle;
      }

      Handle<Scene> LoadScene(Container::String sceneFile)
      {
        auto handle = Factory::Create<Scene>("Scene");
        return handle;
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
            auto light = lightComponent->Get<Light>();

            switch (light->GetLightType())
            {
              case Light::LightType::DIRECTIONAL:
                sceneLights.dirLights.emplace_back(lightComponent);
              break;

              case Light::LightType::POINT:
                sceneLights.pointLights.emplace_back(lightComponent);
              break;

              case Light::LightType::SPOTLIGHT:
                sceneLights.spotLights.emplace_back(lightComponent);
              break;
            }
            found = true;
          }
        }

        return found;
      }

      Material& GetDefaultMaterial(void)
      {
        return g_defaultMaterial;
      }
    }
  }
}