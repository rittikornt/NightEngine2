/*!
  @file Rigidbody.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Rigidbody
*/

#include "Core/EC/Components/Rigidbody.hpp"

#include "Core/EC/GameObject.hpp"

#include "Core/Macros.hpp"

//Physics
#include <btBulletCollisionCommon.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

#include "Physics/PhysicsScene.hpp"
#include "Physics/PhysicUtilities.hpp"
#include "Physics/Collider.hpp"

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      INIT_REFLECTION_FOR(Rigidbody)

      void Rigidbody::OnAwake(void)
      {

      }

      void Rigidbody::Initialize(Physics::PhysicsScene& scene, glm::vec3 initPosition
        , Physics::Collider& collider, float mass)
      {
        ASSERT_TRUE(m_rigidBody == nullptr && m_scene == nullptr);

        m_scene = &scene;

        //Store ColliderShape
        auto collisionShape = collider.CreateCollisionShape();
        m_collider = collider.CreateCollider();
        scene.AddCollisionShape(*collisionShape);

        //Transform
        btTransform initTransform;
        initTransform.setIdentity();
        initTransform.setOrigin(Physics::ToBulletVec3(initPosition));
        
        //Mass & Inertia
        m_mass = mass;
        m_static = (mass == 0.0f);

        btVector3 localInertia(0, 0, 0);
        if (!m_static)
        {
          collisionShape->calculateLocalInertia(mass, localInertia);
        }

        //Create and Add RigidBody to Scene
        btDefaultMotionState* motionState = new btDefaultMotionState(initTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState
          , collisionShape, localInertia);
        m_rigidBody = new btRigidBody(rbInfo);

        //TODO: Find a better way to do this
        //Storing Handle and SlotmapID data into btRigidBody's UserPointer/UserIndex
        auto handle = GetHandle();
        m_rigidBody->setUserPointer(handle.m_lookupFN);         //Pointer to function won't be invalid
        m_rigidBody->setUserIndex(handle.m_slotmapID.m_index);  //Potentially conversion issue?
        m_rigidBody->setUserIndex2(handle.m_slotmapID.m_generation);

        //TODO: use Handle<Rigidbody> instead
        scene.AddRigidBody(*this);
      }

      NightEngine::EC::Components::Transform* Rigidbody::GetTransform(void)
      {
        return m_gameObject->GetTransform();
      }

      Physics::CollisionResult* Rigidbody::GetCollisionResult(void) const
      {
        ASSERT_TRUE(m_gameObject != nullptr);
        return m_scene->GetCollisionResult(*m_gameObject);
      }

      void Rigidbody::SetKinematic(bool kinematic)
      {
        m_isKinematic = kinematic;
        if (kinematic)
        {
          m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() |
            btCollisionObject::CF_KINEMATIC_OBJECT);
          m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
          m_mass = 0.0f;
        }
        else
        {
          m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() &
            ~(btCollisionObject::CF_KINEMATIC_OBJECT) );
          m_rigidBody->setActivationState(ACTIVE_TAG);
          m_mass = 1.0f;
        }
      }

      void Rigidbody::OnDestroy(void)
      {
        //TODO: Remove itself from PhysicScene
        if (m_rigidBody != nullptr)
        {
          //Manually Remove one by one
          m_scene->RemoveRigidBody(*this);
        }

        if (m_collider != nullptr)
        {
          delete m_collider;
        }
      }

    }
  }
}