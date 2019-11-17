/*!
  @file UnitTest.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of UnitTest
*/
#include "UnitTest/UnitTest.hpp"

#include "Core/EC/Factory.hpp"
#include "Core/EC/GameObject.hpp"
#include "Core/EC/ComponentLogic.hpp"

#include "Core/Logger.hpp"
#include "Core/Utility/Utility.hpp"

//Slotmap
#include "Core/Container/Slotmap.hpp"

//MessageSystem Test
#include "Core/Message/MessageTypeEnum.hpp"
#include "Core/Message/MessageObject.hpp"
#include "Core/Message/IMessageHandler.hpp"
#include "Core/Message/MessageObjectList.hpp"
#include "Core/Message/MessageSystem.hpp"

//Serialization
#include "Core/Serialization/Serialization.hpp"

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <string> 

using namespace Core::Utility;
using namespace Core::Container;
using namespace Core::Factory;
using namespace Core;
using namespace ECS;

namespace UnitTest
{
  //*****************************************************
  // Run Test API
  //*****************************************************
	static Catch::Session g_session; // There must be exactly one instance
	
	int RunTest(std::vector<char*>& argv)
	{
		return RunTest(static_cast<int>(argv.size()),&argv[0]);
	}

	int RunTest(int argc, char * argv[])
	{
	  // writing to session.configData() here sets defaults
	  // this is the preferred way to set them
		//Logging
		Debug::Log << "//**********************************************************************\n"
			         << "// Run UnitTest\n"
			         << "//**********************************************************************\n"
			         << Logger::MessageType::INFO <<"Arguments: (";
		for (int i = 0; i < argc; ++i)
		{
			if (i > 0)
			{
				Debug::Log << ", ";
			}

			Debug::Log << argv[i];
		}
		Debug::Log << ")\n\n";

    //New configData()
    g_session.configData() = Catch::ConfigData();

		int returnCode = g_session.applyCommandLine(argc, argv);
		if (returnCode != 0) // Indicates a command line error
		{
      Debug::Log << Logger::MessageType::ERROR_MSG << "Command line error\n";
			return returnCode;
		}

		// writing to session.configData() or session.Config() here 
		// overrides command line args
		// only do this if you know you need to
		int numFailed = 0;
		PROFILE_BLOCK("Run UnitTest")
		{
			numFailed = g_session.run();
		}
    Debug::Log << "//**********************************************************************\n"
               << "// End UnitTest\n"
               << "//**********************************************************************\n";
    
		// numFailed is clamped to 255 as some unices only use the lower 8 bits.
		// This clamping has already been applied, so just return it here
		// You can also do any post run clean-up here
		return numFailed;
	}

  //*****************************************************
  // UnitTest: Message
  //*****************************************************
	namespace Message
	{
		class TestMessageHandler : public IMessageHandler
		{
		public:
			virtual void HandleMessage(const TestMessage& msg) override
			{
				if (msg.m_toggle)
				{
					m_toggle = !m_toggle;
				}
				m_count += msg.m_incrementAmount;
			}
			bool m_toggle = false;
			int m_count = 0;
		};

		TEST_CASE("MessageSystem", "[message]")
		{
			TestMessageHandler handler;
			handler.Subscribe(MessageType::MSG_TEST);

			SECTION("SendMessage_Direct")
			{
				bool flag = handler.m_toggle;

				TestMessage msg(true, 2);
				msg.SendMessageTo(handler);

				REQUIRE(handler.m_count == 2);
				REQUIRE(handler.m_toggle == !flag);

				flag = handler.m_toggle;
				msg.SendMessageTo(handler);

				REQUIRE(handler.m_count == 4);
				REQUIRE(handler.m_toggle == !flag);
			}

			SECTION("Broadcast_MessageSystem")
			{
				bool flag = handler.m_toggle;

				TestMessage msg(true, 2);
				MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);

				REQUIRE(handler.m_count == 2);
				REQUIRE(handler.m_toggle == !flag);

				flag = handler.m_toggle;
				MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);

				REQUIRE(handler.m_count == 4);
				REQUIRE(handler.m_toggle == !flag);

				//More Handler
				TestMessageHandler handler2, handler3, handler4;
				handler2.Subscribe(MessageType::MSG_TEST);
				handler3.Subscribe(MessageType::MSG_TEST);
				handler4.Subscribe(MessageType::MSG_TEST);

				MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);
				MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);
				flag = handler2.m_toggle;
				MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);

				REQUIRE(handler2.m_count == 6);
				REQUIRE(handler3.m_count == 6);
				REQUIRE(handler4.m_count == 6);
				REQUIRE(handler2.m_toggle == !flag);
				REQUIRE(handler3.m_toggle == !flag);
				REQUIRE(handler4.m_toggle == !flag);

				handler2.UnsubscribeAll();
				handler3.UnsubscribeAll();
				handler4.UnsubscribeAll();
			}
			handler.UnsubscribeAll();
		}
	}

  //*****************************************************
  // UnitTest: SlotMap
  //*****************************************************

  void CreateSlotmap(Slotmap<int>& slotmap, SlotmapID* id,int idSize)
  {
    for (int i = 0; i < idSize; ++i)
    {
      id[i] = slotmap.CreateSlot();
      (*slotmap.Get(id[i])) = i;

      if (i > 0)
      {
        //Destroy i times then Create i times
        for (int j = 0; j < i; ++j)
        {
          slotmap.Destroy(id[i - j]);
        }
        for (int j = 0; j < i; ++j)
        {
          id[i - j] = slotmap.CreateSlot();
          (*slotmap.Get(id[i - j])) = -(i - j);
        }
      }
    }
  }

  void DestroySlotmap(Slotmap<int>& slotmap, SlotmapID* id, int begin, int end)
  {
    for (int i = begin; i < end; ++i)
    {
      slotmap.Destroy(id[i]);
    }
  }

	TEST_CASE("Slotmap_int", "[slotmap]")
	{
		const int reserve = 5000;
		const int expandRate = 50;

		Slotmap<int> slotmap(reserve, expandRate);

		REQUIRE(slotmap.Size() == 0);

		SECTION("Create/Destroy_Order_50")
		{
			const int idSize = 50;
			SlotmapID id[idSize];
			for (int i = 0; i < idSize; ++i)
			{
				id[i] = slotmap.CreateSlot();
				(*slotmap.Get(id[i])) = i;

				if (i % 5 == 0)
				{
					REQUIRE(*slotmap.Get(id[i]) == i);
				}
			}

			REQUIRE(slotmap.Size() == idSize);

      //Destroy all
      DestroySlotmap(slotmap, id, 0, idSize);
      REQUIRE(slotmap.Size() == 0);
		}

		SECTION("Create/Destroy_Order_5000")
		{
			const int idSize = 5000;
			SlotmapID id[idSize];
			for (int i = 0; i < idSize; ++i)
			{
				id[i] = slotmap.CreateSlot();
				(*slotmap.Get(id[i])) = i;
			}

			REQUIRE(slotmap.Size() == idSize);

      //Destroy all
      DestroySlotmap(slotmap, id, 0, idSize);
      REQUIRE(slotmap.Size() == 0);
		}

		SECTION("Create/Destroy_NotOrder_100")
		{
			const int idSize = 100;
			SlotmapID id[idSize];

      //Create Slotmap
      CreateSlotmap(slotmap,id, idSize);
			REQUIRE(slotmap.Size() == idSize);

      //Destroy all
      DestroySlotmap(slotmap, id,0, idSize);
      REQUIRE(slotmap.Size() == 0);
		}

    SECTION("Iterator_Access_100")
    {
      //Similar to Create/Destroy_NotOrder_100 Setup
      const int idSize = 100;
      SlotmapID id[idSize];
      //Create Slotmap
      CreateSlotmap(slotmap, id, idSize);
      REQUIRE(slotmap.Size() == idSize);

      //Test Access Loops
      PROFILE_BLOCK_SINGLELINE("Iterator_Access_Full")
      {
        for (int j = 0; j < 100; ++j)
        {
          auto it = slotmap.GetIterator();
          while (!(it.IsEnd()))
          {
            auto val = it.GetNext();
            if (val != nullptr)
            {
              *val = 0;
            }
          }
        }
        Debug::Log << '\n';
      }

      //Destroy half
      DestroySlotmap(slotmap, id, 0, idSize * 0.5f);
      REQUIRE(slotmap.Size() == idSize * 0.5f);

      //Test Access Loops
      PROFILE_BLOCK_SINGLELINE("Iterator_Access_Half")
      {
        for (int j = 0; j < 100; ++j)
        {
          auto it = slotmap.GetIterator();
          while (!(it.IsEnd()))
          {
            auto val = it.GetNext();
            if (val != nullptr)
            {
              *val = 0;
            }
          }
        }
        Debug::Log << '\n';
      }

      //Destroy quarter
      DestroySlotmap(slotmap, id, idSize * 0.5f, idSize * 0.75f);
      REQUIRE(slotmap.Size() == idSize * 0.25f);

      //Test Access Loops
      PROFILE_BLOCK_SINGLELINE("Iterator_Access_Quarter")
      {
        for (int j = 0; j < 100; ++j)
        {
          auto it = slotmap.GetIterator();
          while (!(it.IsEnd()))
          {
            auto val = it.GetNext();
            if (val != nullptr)
            {
              *val = 0;
            }
          }
        }
        Debug::Log << '\n';
      }
      
      //Destroy quarter
      DestroySlotmap(slotmap, id, idSize * 0.75f, idSize);
      REQUIRE(slotmap.Size() == 0);
    }

    SECTION("GetArray_Access_100")
    {
      //Similar to Create/Destroy_NotOrder_100 Setup
      const int idSize = 100;
      SlotmapID id[idSize];
      //Create Slotmap
      CreateSlotmap(slotmap, id, idSize);
      REQUIRE(slotmap.Size() == idSize);

      PROFILE_BLOCK_SINGLELINE("GetArray_Access_Full")
      {
        auto& _array = slotmap.GetArray();
        for (int j = 0; j < 100; ++j)
        {
          for (int i = 0; i < _array.size(); ++i)
          {
            if (_array[i].first.m_active)
            {
              _array[i].second = 0;
            }
          }
        }
      }

      //Destroy half
      DestroySlotmap(slotmap, id, 0, idSize * 0.5f);
      REQUIRE(slotmap.Size() == idSize * 0.5f);

      PROFILE_BLOCK_SINGLELINE("GetArray_Access_Half")
      {
        auto& _array = slotmap.GetArray();
        for (int j = 0; j < 100; ++j)
        {
          for (int i = 0; i < _array.size(); ++i)
          {
            if (_array[i].first.m_active)
            {
              _array[i].second = 0;
            }
          }
        }
      }

      //Destroy quarter
      DestroySlotmap(slotmap, id, idSize * 0.5f, idSize * 0.75f);
      REQUIRE(slotmap.Size() == idSize * 0.25f);

      PROFILE_BLOCK_SINGLELINE("GetArray_Access_Quarter")
      {
        auto& _array = slotmap.GetArray();
        for (int j = 0; j < 100; ++j)
        {
          for (int i = 0; i < _array.size(); ++i)
          {
            if (_array[i].first.m_active)
            {
              _array[i].second = 0;
            }
          }
        }
      }

      //Destroy quarter
      DestroySlotmap(slotmap, id, idSize * 0.75f, idSize);
      REQUIRE(slotmap.Size() == 0);
    }
	}

  //*****************************************************
  // UnitTest: ComponentLogic
  //*****************************************************
	TEST_CASE("ComponentLogic", "[componentlogic]")
	{
		Handle<Controller> controller_1, controller_2;
		Handle<CharacterInfo> character_1, character_2;
		Handle<CTimer> timer_1, timer_2;

		//Create
		controller_1	= Core::Factory::Create<Controller>("Controller");
		controller_2	= Core::Factory::Create<Controller>("Controller");
		character_1		= Core::Factory::Create<CharacterInfo>("CharacterInfo");
		character_2		= Core::Factory::Create<CharacterInfo>("CharacterInfo");
		timer_1				= Core::Factory::Create<CTimer>("CTimer");
		timer_2				= Core::Factory::Create<CTimer>("CTimer");

		SECTION("ComponentUID_Correctness")
		{
			REQUIRE(controller_1->GetUID() != controller_2->GetUID());
			REQUIRE(controller_1->GetUID() != character_1->GetUID());
			REQUIRE(controller_1->GetUID() != character_2->GetUID());
			REQUIRE(controller_1->GetUID() != timer_1->GetUID());
			REQUIRE(controller_1->GetUID() != timer_2->GetUID());
		}

		SECTION("ComponentHandle_Access_Correctness")
		{
			character_1->SetMoveSpeed(10.0f);
			character_2->SetMoveSpeed(5.0f);

			REQUIRE(character_1->GetMoveSpeed() == 10.0f);
			REQUIRE(character_2->GetMoveSpeed() == 5.0f);
		}

		//Destroy
		controller_1.Destroy();
		controller_2.Destroy();
		character_1	.Destroy();
		character_2	.Destroy();
		timer_1			.Destroy();
		timer_2			.Destroy();
	}

  //*****************************************************
  // UnitTest: GameObject
  //*****************************************************
	TEST_CASE("GameObject", "[gameobject]")
	{
		const int gameObjSize = 20;
		Handle<GameObject> gameobj[gameObjSize];

		SECTION("Create/Destroy")
		{
			//Create All
			auto& container = Core::Factory::GetTypeContainer<GameObject>();
      size_t size = container.Size();

			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i] = Core::Factory::Create<GameObject>("GameObject");
			}

			//Destroy All
			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i].Destroy();
			}

			//Should remain the same size
			REQUIRE(size == container.Size());
		}

		SECTION("Add/Get/Remove_Components")
		{
			//Create All
			auto& container = Core::Factory::GetTypeContainer<GameObject>();
			size_t size = container.Size();

			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i] = Core::Factory::Create<GameObject>("GameObject");
				gameobj[i]->AddComponent("CharacterInfo");
				gameobj[i]->AddComponent("Controller");
			}

			for (int i = 0; i < gameObjSize; ++i)
			{
				auto& g = *gameobj[i];
				auto c = g.GetComponent<CharacterInfo>()->Get<CharacterInfo>();
				auto ct = g.GetComponent<Controller>()->Get<Controller>();

				REQUIRE(c != nullptr);
				REQUIRE(ct != nullptr);

				g.RemoveComponent<Controller>();
				REQUIRE(g.GetComponent<Controller>() == nullptr);

				g.RemoveComponent<CharacterInfo>();
				REQUIRE(g.GetComponent<CharacterInfo>() == nullptr);
			}

			//Destroy All
			for (int i = 0; i < gameObjSize; ++i)
			{
        gameobj[i].Get()->RemoveAllComponents();
				gameobj[i].Destroy();
			}

			//Should remain the same size
			REQUIRE(size == container.Size());
		}

		SECTION("HandleAccess_Message")
		{
			//Create All
			auto& container = Core::Factory::GetTypeContainer<GameObject>();
      size_t size = container.Size();

			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i] = Core::Factory::Create<GameObject>("GameObject");
				gameobj[i]->Subscribe(MessageType::MSG_PLAYERUPDATE);

				std::string name{ "GameObject" };
				name += std::to_string(i);
				gameobj[i]->SetName(name);
			}

			PlayerUpdateMessage msg(PlayerUpdateMessage::UpdateType::HEALTHCHANGED, 10);
			MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::GLOBAL);

			//Destroy All
			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i]->UnsubscribeAll();
				gameobj[i].Destroy();
			}

			//Should remain the same size
			REQUIRE(size == container.Size());
		}

		SECTION("Container_Access_Loop")
		{
			//Loops over the internal container
			auto& container = Core::Factory::GetTypeContainer<GameObject>();
      size_t size = container.Size();

			auto& _array = container.GetArray();

			//Create
			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i] = Core::Factory::Create<GameObject>("GameObject");
				
				std::string name{ "GameObject" };
				name += std::to_string(i);
				gameobj[i]->SetName(name);
			}

			//Loop over all internal array and print name
			/*for (int i = 0; i < _array.size(); ++i)
			{
				if (_array[i].first.m_active)
				{
					Debug::Log << _array[i].second.GetName() << '\n';
				}
			}*/

			//Destroy All
			for (int i = 0; i < gameObjSize; ++i)
			{
				gameobj[i].Destroy();
			}

			//Should remain the same size
			REQUIRE(size == container.Size());
		}
	}

  //*****************************************************
  // UnitTest: Reflection
  //*****************************************************
	namespace Reflection
	{
		TEST_CASE("Reflection", "[reflection]")
		{
			//Registration must be done at ReflectionCore::Initialize()
			SECTION("Primitive_GetMetaType")
			{
				int intVar;
				std::string intStr{ "int" };

				REQUIRE(METATYPE(int)->GetName() == "int");
				REQUIRE(METATYPE_FROM_OBJECT(intVar)->GetName() == "int");
				REQUIRE(METATYPE_FROM_STRING("int")->GetName() == "int");
				REQUIRE(METATYPE_FROM_STRING(intStr)->GetName() == "int");

				REQUIRE(METATYPE(int)->GetSize() == sizeof(int));
				REQUIRE(METATYPE_FROM_OBJECT(intVar)->GetSize() == sizeof(int));
				REQUIRE(METATYPE_FROM_STRING("int")->GetSize() == sizeof(int));
				REQUIRE(METATYPE_FROM_STRING(intStr)->GetSize() == sizeof(int));
			}

			SECTION("Class_GetMetaType")
			{
				TestReflection tr;
				std::string trStr{ "TestReflection" };

				LOGINFO_METATYPE(TestReflection);
				
				REQUIRE(METATYPE(TestReflection)->GetName() == "TestReflection");
				REQUIRE(METATYPE_FROM_OBJECT(tr)->GetName() == "TestReflection");
				REQUIRE(METATYPE_FROM_STRING("TestReflection")->GetName() == "TestReflection");
				REQUIRE(METATYPE_FROM_STRING(trStr)->GetName() == "TestReflection");
			}
		}
	}

  //*****************************************************
  // UnitTest: Serialization
  //*****************************************************
	namespace Serialization
	{
    bool debugPrint = false;

		TEST_CASE("Serialization", "[serialization]")
		{
			SECTION("Primitive_Test")
			{
				//Test Primitive Serialization
				int i = 22222;
				Core::Serialization::SerializeToFile(i, "UnitTest_Serialization_Int.json"
					, FileSystem::DirectoryType::Assets);
				float f = 55.444f;
				Core::Serialization::SerializeToFile(f, "UnitTest_Serialization_Float.json"
					, FileSystem::DirectoryType::Assets);
				unsigned u = 255u;
				Core::Serialization::SerializeToFile(u, "UnitTest_Serialization_Unsigned.json"
					, FileSystem::DirectoryType::Assets);
				bool b = false;
				Core::Serialization::SerializeToFile(b, "UnitTest_Serialization_Boolean.json"
					, FileSystem::DirectoryType::Assets);

				//Test Primitive Deserialization
				int dI = 0;
				float dF = 1.0f;
				unsigned dU = 5u;
				bool dB = true;

				//Not Equal on serialize
				REQUIRE(i != dI);
				REQUIRE(f != dF);
				REQUIRE(u != dU);
				REQUIRE(b != dB);

				Core::Serialization::Deserialize(dI, "UnitTest_Serialization_Int.json"
					, FileSystem::DirectoryType::Assets);
				Core::Serialization::Deserialize(dF, "UnitTest_Serialization_Float.json"
					, FileSystem::DirectoryType::Assets);
				Core::Serialization::Deserialize(dU, "UnitTest_Serialization_Unsigned.json"
					, FileSystem::DirectoryType::Assets);
				Core::Serialization::Deserialize(dB, "UnitTest_Serialization_Boolean.json"
					, FileSystem::DirectoryType::Assets);

				//Equal after deserialize
				REQUIRE(i == dI);
				REQUIRE(f == dF);
				REQUIRE(u == dU);
				REQUIRE(b == dB);
			}

			SECTION("POD_Struct_Test")
			{
				//Test Class Serialization
				PODStruct pod{ true,-55, 66, 777.77f };
				Core::Serialization::SerializeToFile(pod, "UnitTest_Serialization_PODStruct.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				PODStruct dpod;
				REQUIRE(pod != dpod);

				//Test Class Deserialization
				Core::Serialization::Deserialize(dpod, "UnitTest_Serialization_PODStruct.json"
					, FileSystem::DirectoryType::Assets);
				REQUIRE(pod == dpod);
			}

			SECTION("Class_Test")
			{
				//Test Class Serialization
				ClassWithProtected t{ true, -3, 4u, 5.55f, false,-11,22,33.33f };
				Core::Serialization::SerializeToFile(t, "UnitTest_Serialization_ClassWithProtected.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				ClassWithProtected dT;
				REQUIRE(t != dT);

				//Test Class Deserialization
				Core::Serialization::Deserialize(dT, "UnitTest_Serialization_ClassWithProtected.json"
					, FileSystem::DirectoryType::Assets);
				REQUIRE(t == dT);
			}

			SECTION("DerivedClass_Test")
			{
				//Test Class Serialization
				DerivedClass dc{ {true, -3, 4u, 5.55f, false,-11,22,33.33f},
				{ true,-111,222,333.33f },{ false,-1111,2222,3333.33f } };
				Core::Serialization::SerializeToFile(dc, "UnitTest_Serialization_DerivedClass.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				DerivedClass ddc;
				REQUIRE(dc != ddc);

				//Test Class Deserialization
				Core::Serialization::Deserialize(ddc, "UnitTest_Serialization_DerivedClass.json"
					, FileSystem::DirectoryType::Assets);
        if (debugPrint)
        {
          Core::Serialization::Serialize(ddc, std::cout);
        }
				REQUIRE(dc == ddc);
			}

			SECTION("SecondDerivedClass_Test")
			{
				//Test Class Serialization
				SecondDerivedClass dc{ DerivedClass{ { true, -3, 4u, 5.55f, false,-11,22,33.33f },
				{ true,-111,222,333.33f },{ false,-1111,2222,3333.33f } }, -9999, 3456.34f };
				Core::Serialization::SerializeToFile(dc, "UnitTest_Serialization_SecondDerivedClass.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				SecondDerivedClass ddc;
				REQUIRE(dc != ddc);

				//Test Class Deserialization
				Core::Serialization::Deserialize(ddc, "UnitTest_Serialization_SecondDerivedClass.json"
					, FileSystem::DirectoryType::Assets);
        
        if (debugPrint)
        {
          Core::Serialization::Serialize(ddc, std::cout);
          std::cout << '\n';
        }
				REQUIRE(dc == ddc);

				//Test Class Serialization
				SecondPrivateDerivedClass sdc{ DerivedClass{ { true, -3, 4u, 5.55f, false,-11,22,33.33f },
				{ true,-111,222,333.33f },{ false,-1111,2222,3333.33f } }, -9999, 3456.34f };
				Core::Serialization::SerializeToFile(sdc, "UnitTest_Serialization_SecondPrivateDerivedClass.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				SecondPrivateDerivedClass dsdc;
				REQUIRE(sdc != dsdc);

				//Test Class Deserialization
				Core::Serialization::Deserialize(dsdc, "UnitTest_Serialization_SecondPrivateDerivedClass.json"
					, FileSystem::DirectoryType::Assets);
        if (debugPrint)
        {
          Core::Serialization::Serialize(dsdc, std::cout);
          std::cout << '\n';
        }
				REQUIRE(sdc == dsdc);
			}

			SECTION("ThirdDerivedClass_Test")
			{
				//Test Class Serialization
				ThirdDerivedClass td{ SecondPrivateDerivedClass{DerivedClass(), -111, 5432.10f}
				,-45425, 322.55f };
				Core::Serialization::SerializeToFile(td, "UnitTest_Serialization_ThirdDerivedClass.json"
					, FileSystem::DirectoryType::Assets);

				//Not equal on init
				ThirdDerivedClass dtd;
				REQUIRE(td != dtd);

				//Test Class Deserialization
				Core::Serialization::Deserialize(dtd, "UnitTest_Serialization_ThirdDerivedClass.json"
					, FileSystem::DirectoryType::Assets);
        if (debugPrint)
        {
          Core::Serialization::Serialize(dtd, std::cout);
          std::cout << '\n';
        }
				REQUIRE(td == dtd);
			}
		}
	}

}