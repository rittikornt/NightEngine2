/*!
  @file PhysicsScene.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of PhysicsScene
*/

#include "Physics/PhysicsScene.hpp"

#include <btBulletCollisionCommon.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

#include "Core/Macros.hpp"
#include "Physics/PhysicUtilities.hpp"
#include "Physics/PhysicsDebugDrawer.hpp"

#include "Core/EC/Components/Rigidbody.hpp"
#include "Core/EC/Components/Transform.hpp"

using namespace NightEngine::EC::Components;
using namespace NightEngine;

namespace Physics
{
  std::vector<PhysicsScene*> PhysicsScene::s_physicScenes;

  PhysicsScene::PhysicsScene(void)
  {
    ASSERT_TRUE(m_world == nullptr);

    //Default configuration (memory, collision setup)
    m_collisionConfig = new btDefaultCollisionConfiguration();

    //Create dispatcher using the config
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfig);
    
    //General purpose broadphase
    m_broadPhase = new btDbvtBroadphase();
  
    //Default Solver
    m_constraintSolver = new btSequentialImpulseConstraintSolver();

    //Create World using above settings
    m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher
      , m_broadPhase, m_constraintSolver, m_collisionConfig);

    m_world->setGravity(btVector3(0, -10, 0));

    //Add to Physic Scene Static List
    s_physicScenes.emplace_back(this);
  }

  void PhysicsScene::Initialize(void)
  {
    //Debug drawer
    m_debugDrawer = new PhysicsDebugDrawer();
    m_world->setDebugDrawer(m_debugDrawer);

    const int reserveSize = 50;
    m_collisionMap.reserve(reserveSize);

    //Create Rigidbodies
    {
      //Ground
      /*btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.0)
        , btScalar(50.0), btScalar(50.0)));

      Rigidbody* ground = new Rigidbody();
      ground->Initialize(*this, glm::vec3(0, -56, 0), groundShape, 0.0f);*/

      ////Ball
      //btCollisionShape* sphereShape = new btSphereShape(btScalar(1.0));

      //Rigidbody* ball = new Rigidbody();
      //ball->Initialize(*this, glm::vec3(0, 1000, 0), sphereShape, 1.0f);
    }

  }

  void PhysicsScene::AddCollisionShape(btCollisionShape& shape)
  {
    m_collisionShapes.push_back(&shape);
  }

  void PhysicsScene::AddRigidBody(NightEngine::EC::Components::Rigidbody& rigidbody)
  {
    //Add RigidBody to World
    m_world->addRigidBody(rigidbody.GetBTRigidBody());

    m_rigidbodys.emplace_back(&rigidbody);
  }

  void PhysicsScene::RemoveRigidBody(NightEngine::EC::Components::Rigidbody& rigidbody)
  {
    //Linear Search
    for (int j = m_world->getNumCollisionObjects() - 1; j > -1; --j)
    {
      btCollisionObject* obj = m_world->getCollisionObjectArray()[j];
      btRigidBody* body = btRigidBody::upcast(obj);

      //Search for rigidbody
      if (body != nullptr && body == rigidbody.GetBTRigidBody())
      {
        //Delete Motion State
        auto motionState = body->getMotionState();
        if (motionState != nullptr)
        {
          delete motionState;
        }

        //Delete Obj
        m_world->removeCollisionObject(obj);
        delete obj;
        break;
      }
    }

    //Remove from RigidBody
    for (auto it = m_rigidbodys.begin()
      ; it != m_rigidbodys.end(); ++it)
    {
      if (&rigidbody == *it)
      {
        m_rigidbodys.erase(it);
        break;
      }
    }

    rigidbody.SetBTRigidBody(nullptr);
  }

  CollisionResult* PhysicsScene::GetCollisionResult(const NightEngine::EC::GameObject& gameObject)
  {
    auto& it = m_collisionMap.find(gameObject.GetHandle().m_handle);
    if (it != m_collisionMap.end())
    {
      return &(it->second);
    }

    return nullptr;
  }

  void PhysicsScene::Update(float dt)
  {
    {
      m_world->stepSimulation(dt, m_simulationSubStep);

      //Update positions of all objects
      for (int i = 0; i < m_rigidbodys.size(); ++i)
      {
        auto motionState = m_rigidbodys[i]->GetBTRigidBody()->getMotionState();
        if (!(m_rigidbodys[i]->IsStatic()) && motionState != nullptr)
        {
          btTransform trans;
          motionState->getWorldTransform(trans);

          //Update the Transform
          auto tranform = m_rigidbodys[i]->GetTransform();
          tranform->SetPosition(ToGLMVec3(trans.getOrigin()));
          tranform->SetRotation(ToGLMQuaternion(trans.getRotation()));
          
          //printf("world pos object %d = %f,%f,%f\n", i, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
        }
      }

      //Update Collision Map
      m_collisionMap.clear();
      int numManifolds = m_world->getDispatcher()->getNumManifolds();
      for (int i = 0; i<numManifolds; i++)
      {
        btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        //Lookup Collided GameObject
        auto rigidbody1 = EC::HandleObject::LookupHandle<Rigidbody>(obA->getUserPointer()
          , obA->getUserIndex(), obA->getUserIndex2());
        auto rigidbody2 = EC::HandleObject::LookupHandle<Rigidbody>(obB->getUserPointer()
          , obB->getUserIndex(), obB->getUserIndex2());

        //Update Collision Map
        auto& result = m_collisionMap[rigidbody1->GetGameObject()->GetHandle().m_handle];
        result.m_gameObject = rigidbody2->GetGameObject()->GetHandle();
        result.m_collidePoint.clear();

        result = m_collisionMap[rigidbody2->GetGameObject()->GetHandle().m_handle];
        result.m_gameObject = rigidbody1->GetGameObject()->GetHandle();
        result.m_collidePoint.clear();

        //std::cout << "Overlapped Pair[" << i << "]: " << rigidbody1->GetGameObject()->GetName()
          // << " vs " << rigidbody2->GetGameObject()->GetName() << '\n';

        //Update Contact Points, Cache it somewhere
        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j<numContacts; j++)
        {
          btManifoldPoint& pt = contactManifold->getContactPoint(j);
          if (pt.getDistance()<0.f)
          {
            const btVector3& ptA = pt.getPositionWorldOnA();
            //const btVector3& ptB = pt.getPositionWorldOnB();
            //const btVector3& normalOnB = pt.m_normalWorldOnB;

            result.m_collidePoint.push_back(ptA);
          }
        }
      }

    }

    //Draw the debugger
    m_world->debugDrawWorld();
  }

  void PhysicsScene::DebugDraw(Rendering::CameraObject& cam)
  {
    m_debugDrawer->Draw(cam);
  }

  PhysicsScene::~PhysicsScene(void)
  {
    //Remove all rigidbodies from world
    for (int j = m_world->getNumCollisionObjects() - 1; j > -1; --j)
    {
      btCollisionObject* obj = m_world->getCollisionObjectArray()[j];
      btRigidBody* body = btRigidBody::upcast(obj);
      auto motionState = body->getMotionState();
      if (body != nullptr && motionState != nullptr)
      {
        delete motionState;
      }
      m_world->removeCollisionObject(obj);
      delete obj;
    }

    //Simply Clear RigidBody Pointers
    m_rigidbodys.clear();

    //Delete Collision Shapes
    for (int i = 0; i < m_collisionShapes.size(); ++i)
    {
      btCollisionShape* shape = m_collisionShapes[i];
      m_collisionShapes[i] = nullptr;
      delete shape;
    }

    //Delete Physics Scene
    delete m_debugDrawer;
    delete m_world;
    delete m_constraintSolver;
    delete m_broadPhase;
    delete m_collisionDispatcher;
    delete m_collisionConfig;

    //Remove this PhysicsScene from the global scenes list
    for (auto it = s_physicScenes.begin(); 
      it != s_physicScenes.end(); ++it)
    {
      if (*it == this)
      {
        s_physicScenes.erase(it);
        break;
      }
    }
  }

  PhysicsScene* PhysicsScene::GetPhysicsScene(int sceneIndex)
  {
    ASSERT_TRUE(s_physicScenes.size() > sceneIndex);

    return s_physicScenes[sceneIndex];
  }

}