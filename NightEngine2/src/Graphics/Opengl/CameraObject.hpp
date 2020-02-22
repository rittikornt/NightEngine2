/*!
  @file CameraObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of CameraObject
*/
#pragma once

#include "Core/Logger.hpp"
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Cubemap.hpp"
#include "Graphics/Opengl/Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rendering
{
	struct CameraObject
	{
    enum class CameraType: unsigned
    {
      PERSPECTIVE = 0,
      ORTHOGRAPHIC
    };

    CameraType m_camtype = CameraType::PERSPECTIVE;
		glm::vec3 m_position;
		glm::vec3 m_rotation;

		glm::vec3 m_worldUp;

		//Dir
		glm::vec3 m_dirFront;
		glm::vec3 m_dirRight;
		glm::vec3 m_dirUp;

		//Matrix
		glm::mat4 m_view;
		glm::mat4 m_projection;

    union CameraSize
    {
      float m_fov = 103.0f;
      float m_size;
      CameraSize(void) = default;
      CameraSize(float size) 
        :m_fov(size){}
    } m_camSize;

    float m_aspect = 16.0f / 9.0f;
    float m_near = 0.01f;
    float m_far = 100.0f;

    //! @brief Constructor
    CameraObject(): m_position(glm::vec3(0.0f, 0.0f, 3.0f))
      , m_rotation(glm::vec3(0.0f, -90.0f, 0.0f))
      , m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
      , m_dirFront(glm::vec3(0.0f, 0.0f, -1.0f))
      , m_dirUp(glm::vec3(0.0f, 1.0f, 0.0f))
      , m_view(glm::mat4(1.0f))
      , m_projection(glm::mat4(1.0f))
    {
      //Dir
      m_dirRight = glm::cross(m_dirFront, m_dirUp);
    }

    //! @brief Constructor
    CameraObject(CameraType type, float size
      , glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f)
      , float near_ = 0.01f, float far_ = 100.0f)
      : m_camtype(type), m_position(pos)
      , m_rotation(glm::vec3(0.0f, -90.0f, 0.0f))
      , m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f))
      , m_dirFront(glm::vec3(0.0f, 0.0f, -1.0f))
      , m_dirUp(glm::vec3(0.0f, 1.0f, 0.0f))
      , m_view(glm::mat4(1.0f))
      , m_projection(glm::mat4(1.0f))
      , m_camSize(size)
      , m_near(near_), m_far(far_)
    {
      m_dirRight = glm::cross(m_dirFront, m_dirUp);
    }

    void ApplyCameraInfo(Shader& shader)
    {
      static std::string pos{ "u_cameraInfo.m_position" };
      shader.SetUniform(pos, m_position);
      //shader.SetUniform("u_cameraInfo.m_lookDir", m_dirFront);
    }

    //TODO: Dirty flag for projection/view matrix, only calculate once per frame
    glm::mat4& GetProjectionMatrix(void)
    {
      //Projection
      m_projection = CalculateProjectionMatrix(m_camtype
        , m_camSize.m_fov, static_cast<float>(Window::GetWidth() / Window::GetHeight())
        , m_near, m_far);

      return m_projection;
    }

    void ApplyProjectionMatrix(Shader& shader)
    {
      //Projection
      m_projection = CalculateProjectionMatrix(m_camtype
        , m_camSize.m_fov, static_cast<float>(Window::GetWidth() / Window::GetHeight())
        , m_near, m_far);

      shader.Bind();
      shader.SetUniform("u_projection", m_projection);
      shader.Unbind();
    }

    glm::mat4& GetViewMatix(void)
    {
      m_view = CalculateViewMatrix(m_position, m_dirFront
        , m_worldUp);
      return m_view;
    }

    void ApplyViewMatrix(Shader& shader)
    {
      m_view = CalculateViewMatrix(m_position, m_dirFront
        , m_worldUp);

      shader.Bind();
      shader.SetUniform("u_view", m_view);
      shader.Unbind();
    }

    void ApplyViewMatrix(Cubemap& cubemap)
    {
      //Cast to mat3 to remove Translation
      m_view = CalculateViewMatrix(m_position, m_dirFront
        , m_worldUp);
      glm::mat4 view = glm::mat4(glm::mat3(m_view));

      Shader& shader = cubemap.GetShader();

      shader.Bind();
      shader.SetUniform("u_view", view);
      shader.Unbind();
    }

    //! @brief Translate camera based on its world direction
		void Move(glm::vec3 amount)
		{
			m_position += amount.x * m_dirRight;
			m_position += amount.y * m_dirUp;
			m_position += amount.z * m_dirFront;
		}

    //! @brief Translate camera with absolute position
		void Translate(glm::vec3 pos)
		{
			m_position += pos;
		}

		glm::vec3 CalculateNewDir(glm::vec3 rotation)
		{
			glm::vec3 newDir;
			newDir.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
			newDir.y = sin(glm::radians(rotation.x));
			newDir.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
			return glm::normalize(newDir);
		}

		void Rotate(glm::vec3 rotateAmount)
		{
			m_rotation += rotateAmount;
			if (m_rotation.x > 89.0f)
				m_rotation.x = 89.0f;
			if (m_rotation.x < -89.0f)
				m_rotation.x = -89.0f;

			//Print("Rotation: ", m_rotation);

			m_dirFront = CalculateNewDir(m_rotation);
			m_dirRight = glm::cross(m_dirFront, m_worldUp);
			m_dirUp = glm::cross(m_dirRight, m_dirFront);
			//Print("Front: ",m_dirFront);
			//Print("Up: ",m_dirUp);
			//Print("Right: ",m_dirRight);
		}

		void Print(char* label, glm::vec3 toPrint)
		{
			NightEngine::Debug::Log << label << toPrint.x << ", " << toPrint.y
				<< ", " << toPrint.z << '\n';
		}

    //*******************************************
    // Static Method
    //*******************************************
    static glm::mat4 CalculateViewMatrix(glm::vec3 position
      , glm::vec3 dir, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
    {
      return glm::lookAt(position, position + dir, worldUp);
    }

    static glm::mat4 CalculateProjectionMatrix(CameraType camtype
      ,float size, float aspect, float near_, float far_)
    {
      glm::mat4 projection = glm::mat4(1.0f);

      switch (camtype)
      {
        case CameraType::PERSPECTIVE:
        {
          projection = glm::perspective(glm::radians(size)
            , aspect
            , near_, far_);
          break;
        }
        case CameraType::ORTHOGRAPHIC:
        {
          float aspect = 1.0f / (static_cast<float>(Window::GetWidth() / Window::GetHeight()) * 0.5f);
          projection = glm::ortho(-size * aspect
            , size * aspect
            , -size
            , size
            , near_, far_);
          break;
        }
      }

      return projection;
    }
	};
}