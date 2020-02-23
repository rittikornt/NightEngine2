/*!
  @file SceneManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of SceneManager
*/

#include "Core/EC/SceneManager.hpp"
#include "Core/EC/Scene.hpp"

#include "Core/EC/Factory.hpp"

namespace NightEngine
{
  namespace EC
  {
    namespace SceneManager
    {
      FACTORY_FUNC_IMPLEMENTATION(Scene);

      void Initialize(void)
      {
        Debug::Log << "SceneManager::Initialize\n";

        FACTORY_REGISTER_TYPE_WITHPARAM(Scene, 1, 5);
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
      }

      /////////////////////////////////////////

      Handle<Scene> CreateScene(void)
      {
        auto handle = Factory::Create<Scene>("Scene");
        
        {
          Scene& scene = *(handle.Get());
        }

        return handle;
      }

      Handle<Scene> LoadScene(Container::String sceneFile)
      {
        auto handle = Factory::Create<Scene>("Scene");
        return handle;
      }
    }
  }
}