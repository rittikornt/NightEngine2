/*!
  @file Transform.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Transform
*/
#include "Core/EC/Components/Transform.hpp"
#include "Core/EC/GameObject.hpp"      //m_gameObject

#include "GLM/gtx/euler_angles.hpp"     //glm::eulerAngleYXZ, glm::eulerAngle
#include <glm/gtc/matrix_transform.hpp> //glm::lookat

#include "Core/Message/MessageObjectList.hpp" //Msg contents
#include "Core/Utility/Utility.hpp"     //PROFILE_BLOCK()

namespace Core
{
  namespace ECS
  {
    namespace Components
    {
      Transform::Transform(void)
        : m_position(0.0f), m_scale(1.0f), m_angle(0.0f)
        , m_rotation(), m_modelMatrix(1.0f), m_dirty(false)
      {
      }

      void Transform::OnAwake(void)
      {
        Subscribe("MSG_TRANSFORMMESSAGE");
      }

      const glm::mat4& Transform::CalculateModelMatrix(void)
      {
        m_modelMatrix = CalculateModelMatrix(*this);
        return m_modelMatrix;
      }
      //*************************************************
      // Handle Message
      //*************************************************
      void Transform::HandleMessage(const Core::TransformMessage& msg)
      {
        if (msg.m_targetName != m_gameObject->GetName()) 
        { 
          return; 
        }

        switch (msg.m_type)
        {
          case TransformMessage::TransformType::TRANSLATE:
          {
            Debug::Log << Logger::MessageType::INFO
              << "Translate to: (" << msg.m_amount.x
              << ", " << msg.m_amount.y << ", " << msg.m_amount.z
              << ")\n";
            m_position = msg.m_amount;
            break;
          }
          case TransformMessage::TransformType::ROTATE:
          {
            Debug::Log << Logger::MessageType::INFO
              << "Rotate to: (" << msg.m_amount.x
              << ", " << msg.m_amount.y << ", " << msg.m_amount.z
              << ")\n";
            SetEulerAngle(msg.m_amount);
            break;
          }
          case TransformMessage::TransformType::SCALE:
          {
            Debug::Log << Logger::MessageType::INFO
              << "Scale to: (" << msg.m_amount.x
              << ", " << msg.m_amount.y << ", " << msg.m_amount.z
              << ")\n";
            SetScale(msg.m_amount);
            break;
          }
        }
      }

      //*************************************************
      // Setter
      //*************************************************
      void Transform::SetPosition(const glm::vec3 & position)
      {
        m_position = position;
        m_dirty = true;
      }

      void Transform::SetEulerAngle(const glm::vec3 & angle)
      {
        m_angle = angle;
        m_rotation = glm::toQuat(glm::eulerAngleYXZ(angle.y, angle.x, angle.z));
        m_dirty = true;
      }

      void Transform::SetRotation(const glm::quat & rotation)
      {
        m_rotation = rotation;
        m_angle = glm::eulerAngles(rotation);
        m_dirty = true;
      }

      void Transform::RotateAround(const glm::vec3 & axis, float angle)
      {
        m_rotation = glm::angleAxis(angle, axis) * m_rotation;
        m_angle = glm::eulerAngles(m_rotation);
        m_dirty = true;
      }

      void Transform::SetScale(const glm::vec3 & scale)
      {
        m_scale = scale;
        m_dirty = true;
      }

      //*************************************************
      // Static Method
      //*************************************************
      glm::mat4 Transform::CalculateModelMatrix(const Transform & transform)
      {
        return CalculateModelMatrix(transform.GetPosition()
          , transform.GetRotation(), transform.GetScale());
      }

      glm::mat4 Transform::CalculateModelMatrix(const glm::vec3 & translation
        , const glm::quat & rotation, const glm::vec3 & scale)
      {
        const float& qx = rotation.x;
        const float& qy = rotation.y;
        const float& qz = rotation.z;
        const float& qw = rotation.w;
        float qx2 = 2 * qx * qx;
        float qy2 = 2 * qy * qy;
        float qz2 = 2 * qz * qz;
        float qwqz = qw * qz;
        float qxqy = qx * qy;
        float qwqy = qw * qy;
        float qxqz = qx * qz;
        float qwqx = qw * qx;
        float qyqz = qy * qz;

        return glm::mat4(
          -(qy2 + qz2 - 1) * scale.x    ,   2 * (qwqz + qxqy) * scale.x,
          -2 * (qwqy - qxqz) * scale.x  ,   0,
          -2 * (qwqz - qxqy) * scale.y  ,   -(qx2 + qz2 - 1) * scale.y,
          2 * (qwqx + qyqz) * scale.y   ,   0,
          2 * (qwqy + qxqz) * scale.z   ,   -2 * (qwqx - qyqz) * scale.z,
          -(qx2 + qy2 - 1) * scale.z    ,   0,
          translation.x                 ,   translation.y,
          translation.z                 ,   1);
      }

      glm::mat4 Transform::CalculateViewMatrix(const glm::vec3 & cameraPos, const glm::vec3 & lookDir, const glm::vec3 & up)
      {
        return glm::lookAt(cameraPos,cameraPos + lookDir, up);
      }

      glm::mat4 Transform::CalculateProjectionMatrix(float fov, float aspect, float near, float far)
      {
        return glm::perspective(glm::radians(fov), aspect,near, far);
      }

      glm::mat4 Transform::CalculateOrthoProjectionMatrix(float size, float aspect, float near, float far)
      {
        aspect = 2.0f / aspect;
        return glm::ortho(-size / aspect, size / aspect, -size / 2.0f,
          size / 2.0f, near, far);
      }

    }
  }
}