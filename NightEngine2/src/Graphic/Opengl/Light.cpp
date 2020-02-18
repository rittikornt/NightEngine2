/*!
  @file Light.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Light
*/

#include "Graphic/Opengl/Light.hpp"
#include "Core/EC/GameObject.hpp"

#include "Graphic/Opengl/CameraObject.hpp"

#include <glm/glm.hpp>    //glm::cos, glm::radian

using namespace Core;

namespace Graphic
{
  INIT_REFLECTION_FOR(Light)
  static const std::string g_pointLightStr[] =
  { "u_pointLightInfo[0]", "u_pointLightInfo[1]", "u_pointLightInfo[2]"
   , "u_pointLightInfo[3]", "u_pointLightInfo[4]" };

  static const std::string g_spotLightStr[] =
  { "u_spotLightInfo[0]", "u_spotLightInfo[1]", "u_spotLightInfo[2]"
    , "u_spotLightInfo[3]", "u_spotLightInfo[4]" };

  Light::Light(LightType type, LightInfo info)
  : m_lightType(type), m_lightInfo(info){}

  void Light::Init(LightType type, LightInfo info, int lightIndex)
  {
    m_lightType = type;
    m_lightInfo = info;
    m_lightIndex = lightIndex;

    //init LightSpaceMatrix
    switch (m_lightType)
    {
      case LightType::DIRECTIONAL:
      {
        m_lightSpaceMatrix.reserve(1);
        m_lightSpaceMatrix.emplace_back();
        break;
      }
      case LightType::POINT:
      {
        m_lightSpaceMatrix.reserve(6);
        m_lightSpaceMatrix.emplace_back();
        m_lightSpaceMatrix.emplace_back();
        m_lightSpaceMatrix.emplace_back();
        m_lightSpaceMatrix.emplace_back();
        m_lightSpaceMatrix.emplace_back();
        m_lightSpaceMatrix.emplace_back();
        break;
      }
      case LightType::SPOTLIGHT:
      {
        break;
      }
    }
  }

  void Light::ApplyLightInfo(Shader& shader)
  {
    //TODO: Send Light information to Shader 
    auto t = m_gameObject->GetTransform();

    switch (m_lightType)
    {
      case LightType::DIRECTIONAL:
      {
        //shader.SetUniform("u_dirLightInfo.m_position", t->GetPosition());
        shader.SetUniform("u_dirLightInfo.m_direction", t->GetForward());
        shader.SetUniform("u_dirLightInfo.m_color", m_lightInfo.m_color.m_value);

        shader.SetUniform("u_dirLightInfo.m_intensity", m_lightInfo.m_value.m_intensity);
        break;
      }
      case LightType::POINT:
      {
        shader.SetUniform(g_pointLightStr[m_lightIndex] + ".m_position", t->GetPosition());
        //shader.SetUniform(g_pointLightStr[m_lightIndex] + ".m_direction", t->GetForward());
        shader.SetUniform(g_pointLightStr[m_lightIndex] + ".m_color", m_lightInfo.m_color.m_value);

        shader.SetUniform(g_pointLightStr[m_lightIndex] + ".m_intensity", m_lightInfo.m_value.m_intensity);
        break;
      }
      case LightType::SPOTLIGHT:
      {
        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_position", t->GetPosition());
        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_direction", t->GetForward());
        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_color", m_lightInfo.m_color.m_value);

        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_intensity", m_lightInfo.m_value.m_spotLight.m_spotLightIntensity);
        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_innerCutOff", m_lightInfo.m_value.m_spotLight.m_inner);
        shader.SetUniform(g_spotLightStr[m_lightIndex] + ".m_outerCutOff", m_lightInfo.m_value.m_spotLight.m_outer);
        break;
      }
    }
  }

  glm::mat4& Light::CalculateLightSpaceMatrix(CameraObject camera
    , float size, float near_, float far_)
  {
    ASSERT_TRUE(m_gameObject != nullptr);
    ASSERT_TRUE(m_gameObject->GetTransform() != nullptr);

    auto transform = m_gameObject->GetTransform();
    auto pos = transform->GetPosition();
    auto forward = transform->GetForward();

    //View Matrix
    auto view = CameraObject::CalculateViewMatrix(pos, -forward
      , camera.m_worldUp);

    //Proj Matrix
    auto projection = CameraObject::CalculateProjectionMatrix(
      CameraObject::CameraType::ORTHOGRAPHIC
      , size, camera.m_aspect,camera.m_near, camera.m_far);

    m_lightSpaceMatrix[0] = projection * view;

    return m_lightSpaceMatrix[0];
  }

  std::vector<glm::mat4>& Light::CalculateLightSpaceMatrices(float size, float aspect
    , float near_, float far_)
  {
    ASSERT_TRUE(m_gameObject != nullptr);
    ASSERT_TRUE(m_gameObject->GetTransform() != nullptr);

    std::vector<glm::mat4> lightSpaceMatrices;

    auto transform = m_gameObject->GetTransform();
    auto pos = transform->GetPosition();

    //Proj Matrix
    auto projection = CameraObject::CalculateProjectionMatrix(
      CameraObject::CameraType::PERSPECTIVE
      , size, aspect, near_, far_);

    //View Matrix Right
    auto view = CameraObject::CalculateViewMatrix(pos, glm::vec3(1.0, 0.0, 0.0)
      , glm::vec3(0.0, -1.0, 0.0));
    m_lightSpaceMatrix[0] = (projection * view);

    //Left
    view = CameraObject::CalculateViewMatrix(pos, glm::vec3(-1.0, 0.0, 0.0)
      , glm::vec3(0.0, -1.0, 0.0));
    m_lightSpaceMatrix[1] = (projection * view);

    //Top
    view = CameraObject::CalculateViewMatrix(pos, glm::vec3(0.0, 1.0, 0.0)
      , glm::vec3(0.0, 0.0, 1.0));
    m_lightSpaceMatrix[2] = (projection * view);
    
    //Bottom
    view = CameraObject::CalculateViewMatrix(pos, glm::vec3(0.0, -1.0, 0.0)
      , glm::vec3(0.0, 0.0, -1.0));
    m_lightSpaceMatrix[3] = (projection * view);
   
    //Near
    view = CameraObject::CalculateViewMatrix(pos, glm::vec3(0.0, 0.0, 1.0)
      , glm::vec3(0.0, -1.0, 0.0));
    m_lightSpaceMatrix[4] = (projection * view);
    
    //Far
    view = CameraObject::CalculateViewMatrix(pos, glm::vec3(0.0, 0.0, -1.0)
      , glm::vec3(0.0, -1.0, 0.0));
    m_lightSpaceMatrix[5] = (projection * view);

    return m_lightSpaceMatrix;
  }

  Light::LightInfo::Value::Value(float intensity)
    : m_intensity(intensity)
  {
  }

  Light::LightInfo::Value::Value(float innerAngle, float outerAngle, float intensity)
    : m_spotLight({ innerAngle, outerAngle, intensity }) {}
}