/*!
  @file Collider.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Collider
*/
#pragma once
#include "Core/Reflection/ReflectionMacros.hpp"
#include <glm/vec3.hpp>

//Forward Declaration
class btCollisionShape;
class btRigidBody;

namespace Physics
{
  enum class ColliderType : int
  {
    NONE = -1,
    BOX_COLLIDER = 0,
    SPHERE_COLLIDER = 1,
    CAPSULE_COLLIDER = 2,
    CYLINDER_COLLIDER = 3
  };

  class Collider
  {
  public:
    //!brief Collider Constructor
    Collider(ColliderType type) : m_colliderType(type) {};

    //!brief Get Bullet CollisionShape
    virtual ~Collider(void);

  protected:
    ColliderType      m_colliderType = ColliderType::NONE;
    btCollisionShape* m_collisionShape = nullptr;
  };

  class BoxCollider : public Collider
  {
  public:
    //!brief Default Constructor
    BoxCollider(const BoxCollider& type) = default;

    //!brief BoxCollider Constructor
    BoxCollider(glm::vec3 size);

  protected:
    glm::vec3 m_size;
  };

  class SphereCollider : public Collider
  {
  public:
    //!brief Default Constructor
    SphereCollider(const SphereCollider& type) = default;

    //!brief SphereCollider Constructor
    SphereCollider(float radius);
  protected:
    float m_radius;
  };

  class CapsuleCollider : public Collider
  {
  public:
    //!brief Default Constructor
    CapsuleCollider(const CapsuleCollider& type) = default;

    //!brief CapsuleCollider Constructor
    CapsuleCollider(float radius, float height);
  protected:
    float m_radius;
    float m_height;
  };

  class CylinderCollider : public Collider
  {
  public:
    //!brief Default Constructor
    CylinderCollider(const CylinderCollider& type) = default;

    //!brief CylinderCollider Constructor
    CylinderCollider(glm::vec3 size);
  protected:
    glm::vec3 m_size;
  };

  ///////////////////////////////////////////

  struct ColliderInitializer
  {
    REFLECTABLE_TYPE_BLOCK()
    {
      META_REGISTERER(ColliderInitializer, true, nullptr, nullptr)
        .MR_ADD_MEMBER_PROTECTED(ColliderInitializer, m_colliderType, true)
        .MR_ADD_MEMBER_PROTECTED(ColliderInitializer, m_size, true);
    }

    ColliderType m_colliderType = ColliderType::NONE;
    glm::vec3 m_size; //xy(radius, height)

    static Physics::Collider* CreateCollider(const ColliderInitializer& params);

    static btCollisionShape* CreateCollisionShape(const ColliderInitializer& params);
  };
}