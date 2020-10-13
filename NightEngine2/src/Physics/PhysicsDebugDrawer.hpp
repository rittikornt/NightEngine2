/*!
@file PhysicsDebugDrawer.hpp
@author Rittikorn Tangtrongchit
@brief Contain the Interface of PhysicsDebugDrawer
*/

#pragma once
#include "LinearMath/btIDebugDraw.h"

#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Shader.hpp"
#include <vector>

//Forward Declaration
namespace NightEngine::Rendering::Opengl
{
  struct CameraObject;
}

namespace Physics
{
  struct Line
  {
    glm::vec3 m_from;
    glm::vec3 m_colorFrom;
    glm::vec3 m_to;
    glm::vec3 m_colorTo;
  };

	class PhysicsDebugDrawer : public btIDebugDraw
	{
	public:
	
	  PhysicsDebugDrawer(void);

	   virtual ~PhysicsDebugDrawer(void);
	
     virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

     virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

	   virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	
	   virtual void   reportErrorWarning(const char* warningString);
	
	   virtual void   draw3dText(const btVector3& location, const char* textString);
	
     virtual void   setDebugMode(int debugMode) { m_debugMode = debugMode; }
	
	   virtual int    getDebugMode(void) const { return m_debugMode; }
	
     //!brief Debug Draw Loop
     void           Draw(NightEngine::Rendering::Opengl::CameraObject& cam);

     /////////////////////////////////////////

     void SetDebugDrawActive(bool enable) { m_enable = enable; }

     bool GetDebugDrawActive(void) { return m_enable; }

     static PhysicsDebugDrawer* GetInstance()
     {
       if (s_instance == nullptr)
       {
         s_instance = new PhysicsDebugDrawer();
       }
       return s_instance;
     }

     static void DeleteInstance()
     {
       if (s_instance != nullptr)
       {
         delete s_instance;
       }
       s_instance = nullptr;
     }
	private:
     static PhysicsDebugDrawer* s_instance;
	   int                        m_debugMode;
     bool                       m_enable;
	
     NightEngine::Rendering::Opengl::Shader            m_shader;
     NightEngine::Rendering::Opengl::VertexArrayObject m_vao;
     std::vector<Line>          m_lines;
	};
}