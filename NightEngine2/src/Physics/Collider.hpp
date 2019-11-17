/*!
  @file Collider.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Collider
*/
#pragma once
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

    //!brief Create Bullet CollisionShape
    virtual btCollisionShape* CreateCollisionShape(void) = 0;

    //!brief Create New Collider
    Collider* CreateCollider(void);
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

    //!brief Create Bullet CollisionShape
    virtual btCollisionShape* CreateCollisionShape(void) override;

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

    //!brief Create Bullet CollisionShape
    virtual btCollisionShape* CreateCollisionShape(void) override;
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

    //!brief Create Bullet CollisionShape
    virtual btCollisionShape* CreateCollisionShape(void) override;
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

    //!brief Create Bullet CollisionShape
    virtual btCollisionShape* CreateCollisionShape(void) override;
  protected:
    glm::vec3 m_size;
  };
}