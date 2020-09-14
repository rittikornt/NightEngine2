/*!
  @file SceneManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SceneManager
*/
#pragma once
#include "Core/Container/String.hpp"
#include "Core/Container/Vector.hpp"

#include "Core/EC/Handle.hpp"
#include "Core/EC/ComponentLogic.hpp"

namespace Rendering
{
  class Material;

  namespace Postprocess
  {
    struct PostProcessSetting;
  }
}

namespace NightEngine
{
  namespace EC
  {
    struct SceneLights
    {
      Container::Vector<Handle<GameObject>> dirLights;
      Container::Vector<Handle<GameObject>> pointLights;
      Container::Vector<Handle<GameObject>> spotLights;

      inline void Clear()
      {
        dirLights.clear();
        pointLights.clear();
        spotLights.clear();
      }
    };

    class Scene;
    namespace SceneManager
    {
      //!breif Initialize SceneManager
      void Initialize(void);

      //!breif Update SceneManager
      void Update(float dt);

      //!breif FixedUpdate SceneManager
      void FixedUpdate(void);

      //!breif Terminate SceneManager
      void Terminate(void);

      /////////////////////////////////////////

      //!@brief Initialize Scene Components
      void InitLoadedScene(Scene& scene);

      //!breif Create Empty Scene
      Handle<Scene> CreateEmptyScene(Container::String sceneName);

      //!breif Create Scene
      Handle<Scene> CreateDefaultScene(Container::String sceneName);

      /////////////////////////////////////////

      //!@brief Load scene through ResourceManager
      Handle<Scene> LoadScene(Container::String sceneFile);

      //!@brief Close scene
      void CloseScene(Handle<Scene> scene);

      //!@brief Save scene
      void SaveScene(Handle<Scene> scene);

      //!@brief Save scene
      void SaveSceneAs(Handle<Scene> scene, std::string fileName);

      //!@brief Save all the currently openning scenes
      void SaveAllScenes(void);

      //!@brief Remove gameobject from a scene
      void RemoveGameObjectFromScene(Handle<GameObject> gameObject);

      //!@brief Reregister all meshRenderer
      void ReregisterAllMeshRenderer(void);

      /////////////////////////////////////////

      //!@brief Get all currently openned scenes
      Container::Vector<Handle<Scene>>* GetAllScenes(void);

      //!@brief Get current active scenes
      Handle<Scene> GetActiveScene(void);

      //!@brief Set the current active scenes
      void SetActiveScene(Handle<Scene> scene);

      /////////////////////////////////////////

      void GetUniqueName(Container::String& sceneName);

      bool GetLights(SceneLights& sceneLights);

      Handle<Rendering::Material> GetDefaultMaterial(void);

      Handle<Rendering::Material> GetBillBoardMaterial(void);

      Handle<Rendering::Material> GetErrorMaterial(void);

      /////////////////////////////////////////

      Rendering::Postprocess::PostProcessSetting& GetPostProcessSetting(void);

      void DeletePostProcessSetting(void);
    }
  }
}