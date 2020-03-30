/*!
  @file Factory.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Factory
*/
#include "Core/EC/Factory.hpp"
#include "Core/Logger.hpp"

//Object Type
#include "Core/EC/GameObject.hpp"
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Scene.hpp"

#include "Graphics/Opengl/Light.hpp"
#include "Core/EC/Components/TestComponent.hpp"
#include "Core/EC/Components/Transform.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"

#include "Graphics/Opengl/Material.hpp"

namespace NightEngine
{
	using namespace Container;
  using namespace Rendering;
	using namespace EC;
  using namespace EC::Components;
	namespace Factory
	{
    //**********************************************************
    // Global Variables
    //**********************************************************
    ObjectFactory<HandleObject> g_factory;

		//Creator Definition
		FACTORY_FUNC_IMPLEMENTATION(GameObject);

    //Components
    FACTORY_FUNC_IMPLEMENTATION(Light);
    FACTORY_FUNC_IMPLEMENTATION(Transform);
    FACTORY_FUNC_IMPLEMENTATION(Rigidbody);
    FACTORY_FUNC_IMPLEMENTATION(MeshRenderer);
		FACTORY_FUNC_IMPLEMENTATION(Controller);
		FACTORY_FUNC_IMPLEMENTATION(CharacterInfo);
		FACTORY_FUNC_IMPLEMENTATION(CTimer);

		FACTORY_FUNC_IMPLEMENTATION(Material);

		//**********************************************************
    // Definition
		//**********************************************************

		void Initialize(void)
		{
			Debug::Log << "Factory::Initialize\n";

			//Creator Registration
      FACTORY_REGISTER_TYPE_WITHPARAM(GameObject, 5000, 5000);

      //Components
      FACTORY_REGISTER_TYPE_WITHPARAM(Transform, 5000, 5000);
      FACTORY_REGISTER_TYPE_WITHPARAM(MeshRenderer, 5000, 5000);
      FACTORY_REGISTER_TYPE(Light);
      FACTORY_REGISTER_TYPE_WITHPARAM(Rigidbody,200, 200);
			FACTORY_REGISTER_TYPE(Controller);
			FACTORY_REGISTER_TYPE(CharacterInfo);
			FACTORY_REGISTER_TYPE(CTimer);

			FACTORY_REGISTER_TYPE(Material);

      //TODO: Get the reflection registerer to init this creator
		}

		HandleObject Create(const char* typeName)
		{
			return g_factory.Create(typeName);
		}

		void Terminate(void)
		{
			Debug::Log << "Factory::Terminate\n";
		}
	}
}