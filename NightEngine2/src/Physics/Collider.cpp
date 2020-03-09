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
    if (m_collisionShape != nullptr)
    {
      delete m_collisionShape;
    }
  }

  BoxCollider::BoxCollider(glm::vec3 size)
    : Collider(ColliderType::BOX_COLLIDER), m_size(size)
  {
  }

  SphereCollider::SphereCollider(float radius)
    : Collider(ColliderType::SPHERE_COLLIDER)
    , m_radius(radius)
  {
  }

  CapsuleCollider::CapsuleCollider(float radius, float height)
    : Collider(ColliderType::CAPSULE_COLLIDER)
    , m_radius(radius), m_height(m_height)
  {
  }

  CylinderCollider::CylinderCollider(glm::vec3 size)
    : Collider(ColliderType::CYLINDER_COLLIDER), m_size(size)
  {
  }

  ///////////////////////////////////////////////////////////////////

  INIT_REFLECTION_FOR(ColliderInitializer)

  Physics::Collider* ColliderInitializer::CreateCollider(const ColliderInitializer& params)
  {
    switch (params.m_colliderType)
    {
      case Physics::ColliderType::BOX_COLLIDER:
      {
        return new BoxCollider(params.m_size);
      }
      case Physics::ColliderType::SPHERE_COLLIDER:
      {
        return new SphereCollider(params.m_size.x);
      }
      case Physics::ColliderType::CAPSULE_COLLIDER:
      {
        return new CapsuleCollider(params.m_size.x, params.m_size.y);
      }
      case Physics::ColliderType::CYLINDER_COLLIDER:
      {
        return new CylinderCollider(params.m_size);
      }
    }

    return nullptr;
  }

  btCollisionShape* ColliderInitializer::CreateCollisionShape(const ColliderInitializer& params)
  {
    switch (params.m_colliderType)
    {
    case Physics::ColliderType::BOX_COLLIDER:
      return new btBoxShape(ToBulletVec3(params.m_size));
    case Physics::ColliderType::SPHERE_COLLIDER:
      return new btSphereShape(params.m_size.x);
    case Physics::ColliderType::CAPSULE_COLLIDER:
      return new btCapsuleShape(params.m_size.x , params.m_size.y); //(radius, height)
    case Physics::ColliderType::CYLINDER_COLLIDER:
      return new btCylinderShape(ToBulletVec3(params.m_size));
    }

    return nullptr;
  }

}