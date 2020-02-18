/*!
  @file Transform.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Transform
*/
#pragma once

#include "Core/EC/ComponentLogic.hpp"

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtx/quaternion.hpp"

namespace Core
{
  namespace ECS
  {
    namespace Components
    {
      //! @brief Special Component that live within GameObject
      class Transform: public ComponentLogic
      {
        REFLECTABLE_TYPE_BLOCK()
        {
          META_REGISTERER_WITHBASE(Transform, ComponentLogic
            , InheritType::PUBLIC, true
            , nullptr, nullptr)
            .MR_ADD_MEMBER_PRIVATE(Transform, m_position, true)
            .MR_ADD_MEMBER_PRIVATE(Transform, m_angle, true)
            .MR_ADD_MEMBER_PRIVATE(Transform, m_scale, true)
            .MR_ADD_MEMBER_PRIVATE(Transform, m_rotation, true)
            .MR_ADD_MEMBER_PRIVATE(Transform, m_modelMatrix, false);
        }
      public:
        //! @brief Default Constructor
        Transform(void);

        //! @brief On Awake
        virtual void OnAwake(void) override;
        
        //! @brief Calculate Model Matrix
        const glm::mat4& CalculateModelMatrix(void);
        //*************************************************
        // Handle Message
        //*************************************************
        //! @brief Handle Message
        virtual void HandleMessage(const Core::TransformMessage& msg) override;

        //*************************************************
        // Getter
        //*************************************************
        //! @brief Get position
        const glm::vec3& GetPosition(void) const { return m_position;}

        //! @brief Get rotation
        const glm::quat& GetRotation(void) const { return m_rotation; }

        //! @brief Get euler angle
        const glm::vec3& GetEulerAngle(void) const { return m_angle; }

        //! @brief Get scale
        const glm::vec3& GetScale(void) const { return m_scale; }

        //! @brief Get Right direction
        glm::vec3 GetRight(void) const { return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f); }

        //! @brief Get Up direction
        glm::vec3 GetUp(void) const { return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

        //! @brief Get Forward direction
        glm::vec3 GetForward(void) const { return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }

        //*************************************************
        // Setter
        //*************************************************
        //! @brief Set position
        void SetPosition(const glm::vec3& position);

        //! @brief Set euler angle
        void SetEulerAngle(const glm::vec3& angle);

        //! @brief Set rotation
        void SetRotation(const glm::quat& rotation);

        //! @brief Rotate around axis for some angle
        void RotateAround(const glm::vec3& axis, float angle);

        //! @brief Set Scale
        void SetScale(const glm::vec3& scale);

        //*************************************************
        // Static Method
        //*************************************************
        //! @brief Calculate Model Matrix from transform
        static glm::mat4 CalculateModelMatrix(const Transform& transform);

        //! @brief Calculate Model Matrix from given info
        static glm::mat4 CalculateModelMatrix(const glm::vec3& translation
          , const glm::quat& rotation, const glm::vec3& scale);

        //! @brief Calculate View Matrix from given info
        static glm::mat4 CalculateViewMatrix(const glm::vec3& cameraPos
          , const glm::vec3& lookDir, const glm::vec3& up);

        //! @brief Calculate Perspective Projection Matrix from given info
        static glm::mat4 CalculateProjectionMatrix(float fov, float aspect
        , float near, float far);

        //! @brief Calculate Orthographic Projection Matrix from given info
        static glm::mat4 CalculateOrthoProjectionMatrix(float size, float aspect
          , float near, float far);
      private:
        glm::vec3 m_position;
        glm::vec3 m_scale;

        glm::vec3 m_angle;
        glm::quat m_rotation;

        glm::mat4 m_modelMatrix;

        bool m_dirty = false;
      };
    }
  }
}