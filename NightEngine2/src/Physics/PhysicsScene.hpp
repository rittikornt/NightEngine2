/*!
  @file PhysicsScene.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PhysicsScene
*/

#pragma once
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btIDebugDraw.h"

#include "Core/ECS/GameObject.hpp"
#include "Core/ECS/Handle.hpp"
#include <vector>
#include <unordered_map>

//Forward Declaration
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;

namespace Core
{
  namespace ECS
  {
    namespace Components
    {
      class Rigidbody;
    }
  }
}

//Forward Declaration
namespace Graphic
{
  struct CameraObject;
}

namespace Physics
{
  class PhysicsDebugDrawer;

  struct CollisionResult
  {
    Core::Factory::Handle<Core::ECS::GameObject> m_gameObject;
    btAlignedObjectArray<btVector3>              m_collidePoint;
  };

  class PhysicsScene
  {
    public:
      //! brief Constructor
      PhysicsScene(void);

      //! brief Initialize
      void Initialize(void);

      //! brief Add Collision Shape
      void AddCollisionShape(btCollisionShape& shape);

      //! brief Add RigidBody to the Scene
      void AddRigidBody(Core::ECS::Components::Rigidbody& rigidbody);

      //! brief Remove RigidBody from the Scene
      void RemoveRigidBody(Core::ECS::Components::Rigidbody& rigidbody);

      //! brief Get Collision Result
      CollisionResult* GetCollisionResult(const Core::ECS::GameObject& gameObject);

      //! brief Update the Scene
      void Update(float dt);

      //! brief Draw the Debug Colliders
      void DebugDraw(Graphic::CameraObject& cam);

      //! brief Destructor
      ~PhysicsScene(void);

      //! brief Get Global PhysicsScene
      static PhysicsScene* GetPhysicsScene(int sceneIndex);
    private:
      int                                     m_simulationSubStep = 10;

      btDefaultCollisionConfiguration*        m_collisionConfig = nullptr;
      btCollisionDispatcher*                  m_collisionDispatcher = nullptr;
      btBroadphaseInterface*                  m_broadPhase = nullptr;
      btSequentialImpulseConstraintSolver*    m_constraintSolver = nullptr;
      btDiscreteDynamicsWorld*                m_world = nullptr;
      
      PhysicsDebugDrawer*                     m_debugDrawer = nullptr;

      std::vector<Core::ECS::Components::Rigidbody*> m_rigidbodys;
      btAlignedObjectArray<btCollisionShape*>        m_collisionShapes;

      //Collision Pair that update every physic tick
      std::unordered_map<Core::Factory::HandleObject, CollisionResult
        , Core::Factory::HandleObjectHash>           m_collisionMap;

      //All existing physic scene
      static std::vector<PhysicsScene*>              s_physicScenes;
  };
}