/*!
  @file ReflectionCore.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ReflectionCore
*/
#include <iostream>

#include "Core/Reflection/ReflectionCore.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/Reflection/MetaManager.hpp"
#include "Core/Logger.hpp"

#include "Core/Utility/Utility.hpp"

// Class for Registration
#include "UnitTest/UnitTest.hpp"

#include "Core/Message/IMessageHandler.hpp"
#include "Core/EC/GameObject.hpp"

//Math
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Graphic/Color.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtx/quaternion.hpp"

//Archetypes
#include "Core/EC/Archetype.hpp"
#include "Core/EC/ArchetypeManager.hpp"

//Components
#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Components/Transform.hpp"
#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Components/MeshRenderer.hpp"

#include "Graphic/Opengl/Light.hpp"
#include "Graphic/Opengl/Material.hpp"

namespace Core
{
  namespace Reflection
  {

    void Initialize()
    {
			Debug::Log << "ReflectionCore::Initialize\n";

      //Invoke all the Reflection initialization functions
      ReflectionInitFunctions::InvokeAll();

			//***********************************************
			//	Primitive/std
			//***********************************************
      using namespace Core::Container;

			REGISTER_METATYPE(int);
			REGISTER_METATYPE(int*);
			REGISTER_METATYPE(int**);
      REGISTER_METATYPE(unsigned long long);
			REGISTER_METATYPE(unsigned);
			REGISTER_METATYPE(float);
			REGISTER_METATYPE(bool);
      REGISTER_METATYPE(std::string);

			REGISTER_METATYPE_WITHNAME(std::vector<int>, "Vector<int>");

      //***********************************************
      //	glm
      //***********************************************
      {
        using namespace glm;
        using namespace Graphic;
        REGISTER_METATYPE(vec2);
        {
          ADD_MEMBER_PUBLIC(vec2, x);
          ADD_MEMBER_PUBLIC(vec2, y);
        }
        REGISTER_METATYPE(vec3);
        {
          ADD_MEMBER_PUBLIC(vec3, x);
          ADD_MEMBER_PUBLIC(vec3, y);
          ADD_MEMBER_PUBLIC(vec3, z);
        }
        REGISTER_METATYPE(vec4);
        {
          ADD_MEMBER_PUBLIC(vec4, x);
          ADD_MEMBER_PUBLIC(vec4, y);
          ADD_MEMBER_PUBLIC(vec4, z);
          ADD_MEMBER_PUBLIC(vec4, w);
        }
        REGISTER_METATYPE(quat);
        {
          ADD_MEMBER_PUBLIC(quat, x);
          ADD_MEMBER_PUBLIC(quat, y);
          ADD_MEMBER_PUBLIC(quat, z);
          ADD_MEMBER_PUBLIC(quat, w);
        }
        REGISTER_METATYPE(mat4);
        REGISTER_METATYPE(Color3);
        {
          ADD_MEMBER_PUBLIC(Color3, m_value);
        }
        REGISTER_METATYPE(Color4);
        {
          ADD_MEMBER_PUBLIC(Color4, m_value);
        }
      }

      //***********************************************
      //	Archetypes
      //***********************************************
      using namespace Core::ECS;
      using namespace Core::ECS::ArchetypeManager;
      REGISTER_METATYPE_WITH_SERIALIZER(Archetype, true
      , SerializeArchetype, DeserializeArchetype);
      {
        ADD_MEMBER_PUBLIC(Archetype, m_name);

        REGISTER_METATYPE_WITHNAME(std::unordered_set<String>
          , "unordered_set<string>");
        ADD_MEMBER_PUBLIC(Archetype, m_componentTypes);
      }

      REGISTER_METATYPE_WITH_SERIALIZER(ArchetypeList, true
      , SerializeArchetypeList, DeserializeArchetypeList);
      {
        REGISTER_METATYPE_WITHNAME(ArchetypeList::ArchetypeMap
          , "map<string, Archetype>");
        ADD_MEMBER_PUBLIC(ArchetypeList, m_archetypes);
      }

			//***********************************************
			//	ECS
			//***********************************************
      using namespace Core::ECS::Components;
      using namespace Core::Serialization;
			REGISTER_METATYPE(IMessageHandler);

      //GameObject with special Serialization Function
      REGISTER_METATYPE_WITH_SERIALIZER(GameObject
      , true, DefaultSerializer<GameObject&>
        , DefaultDeserializer<GameObject&>);
      {
        ADD_MEMBER_PRIVATE(GameObject, m_name);
        ADD_MEMBER_PRIVATE(GameObject, m_components);
        ADD_MEMBER_PRIVATE(GameObject, m_transform);
      }
      REGISTER_METATYPE_WITH_SERIALIZER(GameObject*, false
        , nullptr, nullptr);
      REGISTER_METATYPE_WITH_SERIALIZER(MeshRenderer::DrawMode
        , true, DefaultSerializer<unsigned>
        , DefaultDeserializer<unsigned>);

      //***********************************************
      //	Graphic
      //***********************************************
      using namespace Graphic;
      //LOGINFO_METATYPE(Material);

      //***********************************************
      //	ComponentLogic
      //***********************************************
      REGISTER_METATYPE_WITHBASE(ComponentLogic, IMessageHandler
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PROTECED(ComponentLogic, m_uniqueID, false);
        ADD_MEMBER_PROTECED(ComponentLogic, m_gameObject, false);
      }

      //Basic Components
      //Light
      REGISTER_METATYPE_WITH_SERIALIZER(Light::LightType
      , true, DefaultSerializer<unsigned>, DefaultDeserializer<unsigned>);
      REGISTER_METATYPE(Light::LightInfo);
      {
        REGISTER_METATYPE(Light::LightInfo::Value);
        {
          ADD_MEMBER_PUBLIC(Light::LightInfo::Value, m_intensity);
          ADD_MEMBER_PUBLIC(Light::LightInfo::Value, m_spotLight);
          REGISTER_METATYPE(Light::LightInfo::Value::SpotlightValue);
          {
            ADD_MEMBER_PUBLIC(Light::LightInfo::Value::SpotlightValue, m_inner);
            ADD_MEMBER_PUBLIC(Light::LightInfo::Value::SpotlightValue, m_outer);
            ADD_MEMBER_PUBLIC(Light::LightInfo::Value::SpotlightValue, m_spotLightIntensity);
          }
        }
        ADD_MEMBER_PUBLIC(Light::LightInfo, m_color);
        ADD_MEMBER_PUBLIC(Light::LightInfo, m_value);
      }
      REGISTER_METATYPE_WITHBASE(Light, ComponentLogic
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PRIVATE(Light, m_lightType);
        ADD_MEMBER_PRIVATE(Light, m_lightInfo);
        ADD_MEMBER_PRIVATE(Light, m_lightIndex);
      }

      REGISTER_METATYPE_WITHBASE(Transform, ComponentLogic
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PRIVATE(Transform, m_position);
        ADD_MEMBER_PRIVATE(Transform, m_angle);
        ADD_MEMBER_PRIVATE(Transform, m_scale);
        ADD_MEMBER_PRIVATE(Transform, m_rotation);
        ADD_MEMBER_PRIVATE(Transform, m_modelMatrix, false);
      }
      REGISTER_METATYPE_WITHBASE(Rigidbody, ComponentLogic
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PRIVATE(Rigidbody, m_mass);
        ADD_MEMBER_PRIVATE(Rigidbody, m_static);
      }
      REGISTER_METATYPE(Material*);
      REGISTER_METATYPE_WITHBASE(MeshRenderer, ComponentLogic
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PRIVATE(MeshRenderer, m_material);
        ADD_MEMBER_PRIVATE(MeshRenderer, m_drawMode);
        ADD_MEMBER_PRIVATE(MeshRenderer, m_meshCount);
        ADD_MEMBER_PRIVATE(MeshRenderer, m_castShadow);
      }

      //Test Components
      REGISTER_METATYPE_WITHBASE(Controller, ComponentLogic
        , InheritType::PUBLIC);
      REGISTER_METATYPE_WITHBASE(CharacterInfo, ComponentLogic
        , InheritType::PUBLIC);
      {
        ADD_MEMBER_PRIVATE(CharacterInfo, m_moveSpeed);
        ADD_MEMBER_PRIVATE(CharacterInfo, m_health);
        ADD_MEMBER_PRIVATE(CharacterInfo, m_maxHealth);
      }
      REGISTER_METATYPE_WITHBASE(CTimer, ComponentLogic
        , InheritType::PUBLIC);

			//***********************************************
			//	Unit Test
			//***********************************************
			{
				using namespace UnitTest::Reflection;
				REGISTER_METATYPE(TestReflection);
				{
					ADD_MEMBER_PUBLIC(TestReflection, m_int);
					ADD_MEMBER_PUBLIC(TestReflection, m_float);
					ADD_MEMBER_PUBLIC(TestReflection, m_ints);
					ADD_MEMBER_PUBLIC(TestReflection, m_intPtr);
					ADD_MEMBER_PUBLIC(TestReflection, m_intPtrPtr);
					ADD_MEMBER_PRIVATE(TestReflection, m_private);
					ADD_MEMBER_PRIVATE(TestReflection, m_secret);
				}

				using namespace UnitTest::Serialization;
				REGISTER_METATYPE(PODStruct);
				{
					ADD_MEMBER_PUBLIC(PODStruct, m_pod_bool);
					ADD_MEMBER_PUBLIC(PODStruct, m_pod_int);
					ADD_MEMBER_PUBLIC(PODStruct, m_pod_unsigned);
					ADD_MEMBER_PUBLIC(PODStruct, m_pod_float);
				}
				REGISTER_METATYPE(ClassWithProtected);
				{
					ADD_MEMBER_PUBLIC(ClassWithProtected, m_class_bool);
					ADD_MEMBER_PUBLIC(ClassWithProtected, m_class_int);
					ADD_MEMBER_PUBLIC(ClassWithProtected, m_class_unsigned);
					ADD_MEMBER_PUBLIC(ClassWithProtected, m_class_float);
					ADD_MEMBER_PROTECED(ClassWithProtected, m_class_POD);
				}
				REGISTER_METATYPE_WITHBASE(DerivedClass, ClassWithProtected
					, InheritType::PUBLIC);
				{
					ADD_MEMBER_PROTECED(DerivedClass, m_derived_POD);
					ADD_MEMBER_PROTECED(DerivedClass, m_derived_POD_2);
				}
				REGISTER_METATYPE_WITHBASE(SecondDerivedClass, DerivedClass
					, InheritType::PUBLIC);
				{
					ADD_MEMBER_PROTECED(SecondDerivedClass, m_secondDerived_int);
					ADD_MEMBER_PROTECED(SecondDerivedClass, m_secondDerived_float);
				}
				REGISTER_METATYPE_WITHBASE(SecondPrivateDerivedClass, DerivedClass
					, InheritType::PRIVATE);
				{
					ADD_MEMBER_PROTECED(SecondPrivateDerivedClass, m_secondPrivateDerived_int);
					ADD_MEMBER_PROTECED(SecondPrivateDerivedClass, m_secondPrivateDerived_float);
				}
				REGISTER_METATYPE_WITHBASE(ThirdDerivedClass, SecondPrivateDerivedClass
					, InheritType::PUBLIC);
				{
					ADD_MEMBER_PROTECED(ThirdDerivedClass, m_thridPrivateDerived_int);
					ADD_MEMBER_PROTECED(ThirdDerivedClass, m_thirdPrivateDerived_float);
				}
			}
    }

    void Terminate()
    {
			Debug::Log << "ReflectionCore::Terminate\n";
    }
  }
}
