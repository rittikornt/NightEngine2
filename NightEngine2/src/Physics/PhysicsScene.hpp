/*!
  @file PhysicsScene.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PhysicsScene
*/

#pragma once
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btIDebugDraw.h"

#include "Core/EC/GameObject.hpp"
#include "Core/EC/Handle.hpp"
#include <vector>
#include <unordered_map>

//Forward Declaration
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      class Rigidbody;
    }
  }
}

//Forward Declaration
namespace Rendering
{
  struct CameraObject;
}

namespace Physics
{
  class PhysicsDebugDrawer;

  struct CollisionResult
  {
    NightEngine::EC::Handle<NightEngine::EC::GameObject> m_gameObject;
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
      void AddRigidBody(NightEngine::EC::Components::Rigidbody& rigidbody);

      //! brief Remove RigidBody from the Scene
      void RemoveRigidBody(NightEngine::EC::Components::Rigidbody& rigidbody);

      //! brief Get Collision Result
      CollisionResult* GetCollisionResult(const NightEngine::EC::GameObject& gameObject);

      //! brief Update the Scene
      void Update(float dt);

      //! brief Draw the Debug Colliders
      void DebugDraw(Rendering::CameraObject& cam);

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

      std::vector<NightEngine::EC::Components::Rigidbody*> m_rigidbodys;
      btAlignedObjectArray<btCollisionShape*>        m_collisionShapes;

      //Collision Pair that update every physic tick
      std::unordered_map<NightEngine::EC::HandleObject, CollisionResult
        , NightEngine::EC::HandleObjectHash>           m_collisionMap;

      //All existing physic scene
      static std::vector<PhysicsScene*>              s_physicScenes;
  };
}