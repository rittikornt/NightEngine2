/*!
  @file PhysicUtilities.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PhysicUtilities
*/

#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "GLM/gtx/quaternion.hpp"

#include "LinearMath/btVector3.h"
#include "LinearMath/btTransform.h"

namespace Physics
{
  //! brief Convert GLM mat4 to transform
  inline btTransform ToBulletTransform(const glm::mat4 transform)
  {
    const float* data = glm::value_ptr(transform);
    btTransform bt_transform;

    bt_transform.setFromOpenGLMatrix(data);
    return bt_transform;
  }

  //! brief Convert glm vector3 to Bullet vector3
  inline btVector3 ToBulletVec3(const glm::vec3& glm_vec3)
  {
    return btVector3(glm_vec3.x, glm_vec3.y, glm_vec3.z);
  }

  //! brief Convert Bullet transform to mat4
  inline glm::mat4 ToGLMMat4(const btTransform& transform)
  {
    glm::mat4 glm_transform(1.0f);
    float* data = glm::value_ptr(glm_transform);

    transform.getOpenGLMatrix(data);
    return glm_transform;
  }

  //! brief Convert Bullet vector3 to glm vector3
  inline glm::vec3 ToGLMVec3(const btVector3& bt_vec3)
  {
    return glm::vec3(bt_vec3.getX(), bt_vec3.getY(), bt_vec3.getZ());
  }

  //! brief Convert Bullet Quaternion to glm Quaternion
  inline btQuaternion ToBulletQuaternion(const glm::quat& glm_quat)
  {
    return btQuaternion(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w);
  }

  //! brief Convert Bullet Quaternion to glm Quaternion
  inline glm::quat ToGLMQuaternion(const btQuaternion& bt_quat)
  {
    return glm::quat(bt_quat.w(), bt_quat.x(), bt_quat.y(), bt_quat.z());
  }
}