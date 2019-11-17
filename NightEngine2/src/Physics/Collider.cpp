/*!
  @file Collider.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Collider
*/

#include "Physics/Collider.hpp"

#include "Physics/PhysicUtilities.hpp"
#include <btBulletCollisionCommon.h>

namespace Physics
{
  Collider::~Collider(void)
  {
  }

  Physics::Collider* Collider::CreateCollider(void)
  {
    switch (m_colliderType)
    {
    case Physics::ColliderType::BOX_COLLIDER:
    {
      return new BoxCollider( *(static_cast<BoxCollider*>(this)) );
      break;
    }
    case Physics::ColliderType::SPHERE_COLLIDER:
    {
      return new SphereCollider(*(static_cast<SphereCollider*>(this)));
      break;
    }
    case Physics::ColliderType::CAPSULE_COLLIDER:
    {
      return new CapsuleCollider(*(static_cast<CapsuleCollider*>(this)));
      break;
    }
    case Physics::ColliderType::CYLINDER_COLLIDER:
    {
      return new CylinderCollider(*(static_cast<CylinderCollider*>(this)));
      break;
    }
    default:
    {
      break;
    }
    }

    return nullptr;
  }

  BoxCollider::BoxCollider(glm::vec3 size)
    : Collider(ColliderType::BOX_COLLIDER), m_size(size)
  {
  }

  btCollisionShape* BoxCollider::CreateCollisionShape(void)
  {
    m_collisionShape = new btBoxShape(ToBulletVec3(m_size));
    return m_collisionShape;
  }

  SphereCollider::SphereCollider(float radius)
    : Collider(ColliderType::SPHERE_COLLIDER)
    , m_radius(radius)
  {
  }

  btCollisionShape* SphereCollider::CreateCollisionShape(void)
  {
    m_collisionShape = new btSphereShape(m_radius);
    return m_collisionShape;
  }

  CapsuleCollider::CapsuleCollider(float radius, float height)
    : Collider(ColliderType::CAPSULE_COLLIDER)
    , m_radius(radius), m_height(m_height)
  {
  }

  btCollisionShape* CapsuleCollider::CreateCollisionShape(void)
  {
    m_collisionShape = new btCapsuleShape(m_radius, m_height);
    return m_collisionShape;
  }

  CylinderCollider::CylinderCollider(glm::vec3 size)
    : Collider(ColliderType::CYLINDER_COLLIDER), m_size(size)
  {
  }

  btCollisionShape* CylinderCollider::CreateCollisionShape(void)
  {
    m_collisionShape = new btCylinderShape(ToBulletVec3(m_size));
    return m_collisionShape;
  }

}