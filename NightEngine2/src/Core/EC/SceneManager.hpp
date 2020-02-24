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
      void Update(void);

      //!breif FixedUpdate SceneManager
      void FixedUpdate(void);

      //!breif Terminate SceneManager
      void Terminate(void);

      /////////////////////////////////////////

      //!breif Create Scene
      Handle<Scene> CreateDefaultScene(Container::String sceneName);

      //!@brief Load scene through ResourceManager
      Handle<Scene> LoadScene(Container::String sceneFile);

      //!@brief Initialize Scene Components
      void InitLoadedScene(Scene& scene);

      //!@brief Load scene through ResourceManager
      void SaveScene(Handle<Scene> scene);

      //!@brief Get all currently openned scenes
      Container::Vector<Handle<Scene>>* GetAllScenes(void);

      /////////////////////////////////////////

      bool GetLights(SceneLights& sceneLights);

      Rendering::Material& GetDefaultMaterial(void);

      Rendering::Material& GetBillBoardMaterial(void);
    }
  }
}