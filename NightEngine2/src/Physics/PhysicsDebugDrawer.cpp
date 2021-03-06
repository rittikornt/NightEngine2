/*!
@file PhysicsDebugDrawer.cpp
@author Rittikorn Tangtrongchit
@brief Contain the Implementation of PhysicsDebugDrawer
*/

#include "Physics/PhysicsDebugDrawer.hpp"

#include "Core/Logger.hpp"
#include "Core/EC/Components/Transform.hpp"
#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/PrimitiveShape.hpp"

#include "Physics/PhysicUtilities.hpp"

using namespace NightEngine;
using namespace NightEngine::EC::Components;
using namespace NightEngine::Rendering::Opengl;
using namespace NightEngine::Rendering::Opengl::PrimitiveShape;

namespace Physics
{
  PhysicsDebugDrawer* PhysicsDebugDrawer::s_instance = nullptr;

  PhysicsDebugDrawer::PhysicsDebugDrawer()
  {
    m_lines.emplace_back(Line{ glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,0.0f)
      ,glm::vec3(0.0f, 0.0f, 0.0f) , glm::vec3(0.0f,0.0f,0.0f) });


    //VAO
    float* linePtr = &(m_lines[0].m_from.x);

    AttributePointerInfo info{ 2
      , std::vector<unsigned>{3, 3}
      , std::vector<size_t>{sizeof(glm::vec3), sizeof(glm::vec3)}
      , std::vector<bool>{ false, false}
      , std::vector<int>{ 0, 0} };

    m_vao.Init();
    m_vao.Build(BufferMode::Dynamic, linePtr
      , m_lines.size() * sizeof(Line), info);

    //m_vao.Init();
    //m_vao.Build(BufferMode::Static, Quad::vertices
     // , Quad::indices, Quad::info);

    //Shader
    m_shader.Create();
    m_shader.AttachShaderFile("ShaderPass/Debug/debug_draw.vert");
    m_shader.AttachShaderFile("ShaderPass/Debug/debug_draw.frag");
    m_shader.Link();
  }

  PhysicsDebugDrawer::~PhysicsDebugDrawer()
  {
  }

  void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
  {
    m_lines.emplace_back(Line{ Physics::ToGLMVec3(from), Physics::ToGLMVec3(color)
      , Physics::ToGLMVec3(to), Physics::ToGLMVec3(color) });
  }

  void PhysicsDebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor)
  {
    m_lines.emplace_back(Line{ Physics::ToGLMVec3(from), Physics::ToGLMVec3(fromColor)
      , Physics::ToGLMVec3(to), Physics::ToGLMVec3(toColor) });
  }

  void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
  {

  }

  void PhysicsDebugDrawer::reportErrorWarning(const char* warningString)
  {
    Debug::Log << Logger::MessageType::WARNING << warningString << '\n';
  }

  void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
  {

  }

  void PhysicsDebugDrawer::Draw(CameraObject& cam)
  {
    if (!m_enable)
    {
      return;
    }

    if (m_lines.size() > 0)
    {
      //Build new Lines data
      m_vao.Clear();
      float* linePtr = &(m_lines[0].m_from.x);
      m_vao.Build(BufferMode::Dynamic, linePtr
        , m_lines.size() * sizeof(Line));

      cam.ApplyUnJitteredProjectionMatrix(m_shader);
      cam.ApplyViewMatrix(m_shader);

      //Draw the Colliders
      m_shader.Bind();
      {
        auto model = Transform::CalculateModelMatrix(glm::vec3(0.0f)
          , glm::quat(), glm::vec3(1.0f, 1.0f, 1.0f));
        m_shader.SetUniform("u_model", model);
        m_vao.DrawVBO(DrawMode::LINES);
      }
      m_shader.Unbind();

      //Clear Lines data
      m_lines.clear();
    }
  }

}