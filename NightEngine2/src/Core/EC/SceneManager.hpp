/*!
  @file SceneManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SceneManager
*/
#pragma once
#include "Core/Container/String.hpp"
#include "Core/EC/Handle.hpp"

namespace NightEngine
{
  namespace EC
  {
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

      //!breif GetActiveScenes
      //Scene& GetActiveScenes(void);
      
      //!breif Create Scene
      Handle<Scene> CreateScene(void);

      //!@brief Load scene through ResourceManager
      Handle<Scene> LoadScene(Container::String sceneFile);
    }
  }
}