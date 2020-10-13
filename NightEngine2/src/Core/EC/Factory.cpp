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
#include "Graphics/Opengl/MeshRenderer.hpp"

#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/Model.hpp"

namespace NightEngine
{
	using namespace Container;
  using namespace NightEngine::Rendering::Opengl;
	using namespace EC;
  using namespace EC::Components;
	namespace Factory
	{
    //**********************************************************
    // Global Variables
    //**********************************************************
		HandleObjectFactory g_factory;

		//Creator Definition
		FACTORY_FUNC_IMPLEMENTATION(GameObject);
		FACTORY_FUNC_IMPLEMENTATION(Texture);
		FACTORY_FUNC_IMPLEMENTATION(Model);

		//**********************************************************
    // Definition
		//**********************************************************
		void Initialize(void)
		{
			Debug::Log << "Factory::Initialize\n";

			//Creator Registration
			//NightEngine::Factory::g_factory.Register("GameObject",
			//NightEngine::Factory::HandleObjectFactory::InfoFN{ FactoryCreateGameObject,FactoryLookupGameObject, FactoryDestroyGameObject });
			//NightEngine::Factory::GetTypeContainer<GameObject>().Reserve(5000, 5000);
      FACTORY_REGISTER_TYPE_WITHPARAM(GameObject, 2000, 1000);
			FACTORY_REGISTER_TYPE(Texture);
			FACTORY_REGISTER_TYPE(Model);
		}

		HandleObject Create(const char* typeName)
		{
			return g_factory.Create(typeName);
		}

		void Terminate(void)
		{
			Debug::Log << "Factory::Terminate\n";
			g_factory.Clear();
		}
	}
}