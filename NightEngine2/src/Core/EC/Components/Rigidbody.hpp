/*!
  @file Rigidbody.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Rigidbody
*/
#pragma once

#include "Core/EC/ComponentLogic.hpp"
#include "Core/EC/Factory.hpp"

#include "LinearMath/btVector3.h"
#include <glm/vec3.hpp>
#include "Physics/Collider.hpp"

//Forward Declaration
class btRigidBody;

namespace Physics
{
  class PhysicsScene;
  struct CollisionResult;
  class Collider;
  struct ColliderInitializer;
}

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      class Transform;

      class Rigidbody: public ComponentLogic
      {
        REFLECTABLE_TYPE_BLOCK()
        {
          META_REGISTERER_WITHBASE(Rigidbody, ComponentLogic
            , InheritType::PUBLIC, true
            , nullptr, nullptr)
            .MR_ADD_MEMBER_PROTECTED(Rigidbody, m_colliderParams, true)
            .MR_ADD_MEMBER_PROTECTED(Rigidbody, m_mass, true)
            .MR_ADD_MEMBER_PROTECTED(Rigidbody, m_static, true);
        }
      public:

        //! @brief On Awake
        virtual void OnAwake(void) override;

        //! brief Initialize Rigidbody
        void Initialize(Physics::PhysicsScene& scene, glm::vec3 initPosition
          , const Physics::ColliderInitializer& colliderParams, float mass = 0.0f);

        //! brief set BT RigidBody
        void SetBTRigidBody(btRigidBody* rigidbody) { m_rigidBody = rigidbody; }

        //! brief Get BT RigidBody
        inline btRigidBody* GetBTRigidBody(void) { return m_rigidBody; }

        //! brief Get Reference to Transform
        Transform* GetTransform(void);

        //! brief Get Collision Result
        Physics::CollisionResult* GetCollisionResult(void) const;

        //! brief Get ColliderInitializer
        Physics::ColliderInitializer GetColliderInitializer(void) const { return m_colliderParams; }

        //! brief Set Kinematic mode
        void SetKinematic(bool kinematic);

        //! brief Check if Static or not
        inline bool IsStatic(void) const { return m_static; }

        //! brief OnDestroy Callback
        virtual void OnDestroy(void) override;
      private:
        btRigidBody*               m_rigidBody = nullptr;
        Physics::PhysicsScene*     m_scene = nullptr;
        Physics::Collider*         m_collider = nullptr; //Reference to Collider Information (for serialization)

        Physics::ColliderInitializer m_colliderParams;
        btScalar                   m_mass = 0.0f;
        bool                       m_static = true;
        bool                       m_isKinematic = false;
      };
    }
  }
}