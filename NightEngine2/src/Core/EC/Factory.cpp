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

#include "Graphic/Opengl/Light.hpp"
#include "Core/EC/Components/Transform.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"

#define FACTORY_CREATOR_REGISTER(TYPE) \
	g_factory.Register<TYPE>(&TYPE##Creator);																

#define FACTORY_CREATOR_REGISTER_WITHPARAM(TYPE, RESERVE_INT, EXPAND_RATE) \
	g_factory.Register<TYPE>(&TYPE##Creator); \
	GetTypeContainer<TYPE>().Reserve(RESERVE_INT, EXPAND_RATE); 	

#define FACTORY_CREATOR_DEFINITION(TYPE) \
	void* Lookup##TYPE(const SlotmapID& id) \
	{	\
		return GetTypeContainer<TYPE>().Get(id); \
	}	\
	void Destroy##TYPE(const SlotmapID& id)	\
	{	\
		GetTypeContainer<TYPE>().Destroy(id);	\
	}	\
	HandleObject Create##TYPE()	\
	{	\
		return HandleObject(GetTypeContainer<TYPE>().CreateSlot()	\
					, Lookup##TYPE, Destroy##TYPE);	 \
	}	\
	Creator<HandleObject> TYPE##Creator(Create##TYPE, #TYPE);																			 

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
		FACTORY_CREATOR_DEFINITION(GameObject);

    //Components
    FACTORY_CREATOR_DEFINITION(Light);
    FACTORY_CREATOR_DEFINITION(Transform);
    FACTORY_CREATOR_DEFINITION(Rigidbody);
    FACTORY_CREATOR_DEFINITION(MeshRenderer);
		FACTORY_CREATOR_DEFINITION(Controller);
		FACTORY_CREATOR_DEFINITION(CharacterInfo);
		FACTORY_CREATOR_DEFINITION(CTimer);

		//**********************************************************
    // Definition
		//**********************************************************

		void Initialize(void)
		{
			Debug::Log << "Factory::Initialize\n";

			//Creator Registration
      FACTORY_CREATOR_REGISTER_WITHPARAM(GameObject, 5000, 5000);

      //Components
      FACTORY_CREATOR_REGISTER_WITHPARAM(Transform, 5000, 5000);
      FACTORY_CREATOR_REGISTER_WITHPARAM(MeshRenderer, 5000, 5000);
      FACTORY_CREATOR_REGISTER(Light);
      FACTORY_CREATOR_REGISTER_WITHPARAM(Rigidbody,200, 200);
			FACTORY_CREATOR_REGISTER(Controller);
			FACTORY_CREATOR_REGISTER(CharacterInfo);
			FACTORY_CREATOR_REGISTER(CTimer);
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