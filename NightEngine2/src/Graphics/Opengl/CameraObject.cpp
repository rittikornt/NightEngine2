/*!
  @file CameraObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of CameraObject
*/
#include "Graphics/Opengl/CameraObject.hpp"

#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Cubemap.hpp"
#include "Graphics/Opengl/Window.hpp"

#include "Core/Logger.hpp"
#include "Input/Input.hpp"

#define SCREEN_ASPECT_RATIO static_cast<float>((float)Window::GetWidth() / (float)Window::GetHeight())

namespace Rendering
{
  struct FrustumJitter
  {
    static float GetHaltonSequence(int index, int radix)
    {
      float result = 0.0f;
      float fraction = 1.0f / radix;

      while (index > 0)
      {
        result += (index % radix) * fraction;

        index /= radix;
        fraction /= radix;
      }

      return result;
    }

    static glm::vec4 GetProjectionExtends(const CameraObject& camera
      , float texelOffsetX, float texelOffsetY, float pixelWidth, float pixelHeight)
    {
      float k_deg2Rad = 0.01745329251f; // (PI * 2) / 360
      float aspect = SCREEN_ASPECT_RATIO; //This is only for perspective for now

      float oneExtentY = camera.m_projectionType == CameraObject::CameraType::ORTHOGRAPHIC ? 
        camera.m_camSize.m_size : tanf(0.5f * k_deg2Rad * camera.m_camSize.m_fov);
      float oneExtentX = oneExtentY * aspect;
      float texelSizeX = oneExtentX / (0.5f * pixelWidth);
      float texelSizeY = oneExtentY / (0.5f * pixelHeight);
      float oneJitterX = texelSizeX * texelOffsetX;
      float oneJitterY = texelSizeY * texelOffsetY;

      // xy = frustum extents at distance 1, zw = jitter at distance 1
      return glm::vec4(oneExtentX, oneExtentY, oneJitterX, oneJitterY);
    }

    static glm::mat4 GetPerspectiveProjection(float left, float right
      , float bottom, float top, float near_, float far_)
    {
      //minwindef.h has macros near/far defined, seriously microsoft?
      float x = (2.0f * near_) / (right - left);
      float y = (2.0f * near_) / (top - bottom);
      float a = (right + left) / (right - left);
      float b = (top + bottom) / (top - bottom);
      float c = -(far_ + near_) / (far_ - near_);
      float d = -(2.0f * far_ * near_) / (far_ - near_);
      float e = -1.0f;

      glm::mat4 m;
      m[0][0] = x; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
      m[1][0] = 0; m[1][1] = y; m[1][2] = 0; m[1][3] = 0;
      m[2][0] = a; m[2][1] = b; m[2][2] = c; m[2][3] = e;
      m[3][0] = 0; m[3][1] = 0; m[3][2] = d; m[3][3] = 0;
      return m;
    }

    static glm::mat4 GetJitteredProjection(const CameraObject& camera, glm::vec2& jitteredUV)
    {
      int taaFrameIndex = camera.m_taaFrameIndex;
      float texelOffsetX = GetHaltonSequence((taaFrameIndex & 1023) + 1, 2) - 0.5f;
      float texelOffsetY = GetHaltonSequence((taaFrameIndex & 1023) + 1, 3) - 0.5f;
      
      float pixelWidth = Window::GetWidth();
      float pixelHeight = Window::GetHeight();
      jitteredUV.x = texelOffsetX / pixelWidth;
      jitteredUV.y = texelOffsetX / pixelHeight;

      glm::vec4 extents = GetProjectionExtends(camera
        , texelOffsetX, texelOffsetY, pixelWidth, pixelHeight);

      float cf = camera.m_far;
      float cn = camera.m_near;

      float xm = extents.z - extents.x;
      float xp = extents.z + extents.x;
      float ym = extents.w - extents.y;
      float yp = extents.w + extents.y;

      //TODO: Jitter for Orthographic Projection
      return GetPerspectiveProjection(xm * cn, xp * cn, ym * cn, yp * cn, cn, cf);
    }
  };

  static void Print(char* label, glm::vec3 toPrint)
  {
    NightEngine::Debug::Log << label << toPrint.x << ", " << toPrint.y
      << ", " << toPrint.z << '\n';
  }

  CameraObject::CameraObject() : 
    m_position(DEFAULT_CAM_POS), m_eulerAngle(DEFAULT_ANGLE)
    , m_dirForward(DEFAULT_FORWARD), m_dirUp(WORLD_UP)
    , m_view(glm::mat4()), m_projection(glm::mat4())
  {
    Rotate(VEC3_ZERO);
  }

  CameraObject::CameraObject(CameraType type, float size, glm::vec3 pos
    , float near_, float far_)
    : m_projectionType(type), m_position(pos), m_eulerAngle(DEFAULT_ANGLE)
    , m_dirForward(DEFAULT_FORWARD), m_dirUp(WORLD_UP)
    , m_view(glm::mat4()), m_projection(glm::mat4())
    , m_camSize(size), m_near(near_), m_far(far_)
  {
    Rotate(VEC3_ZERO);
  }

  //////////////////////////////////////////////////////////

  void CameraObject::ApplyCameraInfo(Shader& shader)
  {
    static std::string pos{ "u_cameraInfo.m_position" };
    shader.SetUniform(pos, m_position);
    //shader.SetUniform("u_cameraInfo.m_lookDir", m_dirForward);
  }

  void CameraObject::ApplyUnJitteredProjectionMatrix(Shader& shader)
  {
    //Projection
    m_unjitteredProjection = CalculateProjectionMatrix(m_projectionType
      , m_camSize.m_fov, SCREEN_ASPECT_RATIO
      , m_near, m_far);

    shader.Bind();
    shader.SetUniform("u_projection", m_unjitteredProjection);
    shader.Unbind();
  }

  void CameraObject::ApplyViewMatrix(Shader& shader)
  {
    shader.Bind();
    shader.SetUniform("u_view", GetViewMatix());
    shader.Unbind();
  }

  void CameraObject::ApplyViewMatrix(Cubemap& cubemap)
  {
    //Cast to mat3 to remove Translation
    glm::mat4 view = glm::mat4(glm::mat3(GetViewMatix()));

    Shader& shader = cubemap.GetShader();

    shader.Bind();
    shader.SetUniform("u_view", view);
    shader.Unbind();
  }

  //////////////////////////////////////////////////////////

  void CameraObject::OnStartFrame(void)
  {
    m_view = CalculateViewMatrix(m_position, m_dirForward, WORLD_UP);

    m_unjitteredProjection = CalculateProjectionMatrix(m_projectionType
      , m_camSize.m_fov, SCREEN_ASPECT_RATIO
      , m_near, m_far);

    m_projection = m_bJitterProjectionMatrix? 
      CalculateJitteredProjectionMatrix(*this, m_activeJitteredUV) : m_unjitteredProjection;

    m_invView = glm::inverse(m_view);
    m_invProjection = glm::inverse(m_projection);
    
    //VP
    m_unjitteredVP = m_unjitteredProjection * m_view;
    m_VP = m_projection * m_view; //this could be jittered

    //Update TAAframeIndex
    if (m_bJitterProjectionMatrix)
    {
      const int k_maxSampleCount = 8;
      if (++m_taaFrameIndex >= k_maxSampleCount)
      {
        m_taaFrameIndex = 0;
      }
    }
  }

  void CameraObject::OnEndFrame(void)
  {
    m_prevUnjitteredVP = m_unjitteredVP;
  }

  //////////////////////////////////////////////////////////

  glm::mat4& CameraObject::GetUnjitteredProjectionMatrix(void)
  {
    //TODO: Dirty flag for projection/view matrix, only calculate once per frame
    //Projection
    m_unjitteredProjection = CalculateProjectionMatrix(m_projectionType
      , m_camSize.m_fov, SCREEN_ASPECT_RATIO
      , m_near, m_far);

    return m_unjitteredProjection;
  }

  glm::mat4& CameraObject::GetViewMatix(void)
  {
    m_view = CalculateViewMatrix(m_position, m_dirForward, WORLD_UP);
    //m_view = glm::lookAt(m_position, glm::vec3(0.0f,0.0f,0.0f), WORLD_UP);;
    return m_view;
  }

  //////////////////////////////////////////////////////////

  void CameraObject::Move(glm::vec3 amount)
  {
    m_position += amount.x * m_dirRight;
    m_position += amount.y * m_dirUp;
    m_position += amount.z * m_dirForward;
  }

  void CameraObject::Translate(glm::vec3 pos)
  {
    m_position += pos;
  }

  glm::vec3 CameraObject::CalculateDir(glm::vec3 eulerAngle)
  {
    glm::vec3 newDir;
    newDir.x = cos(glm::radians(eulerAngle.x)) * cos(glm::radians(eulerAngle.y));
    newDir.y = sin(glm::radians(eulerAngle.x));
    newDir.z = cos(glm::radians(eulerAngle.x)) * sin(glm::radians(eulerAngle.y));
    return glm::normalize(newDir);
  }

  void CameraObject::Rotate(glm::vec3 eulerAngle)
  {
    m_eulerAngle += eulerAngle;
    if (m_eulerAngle.x > 89.0f)
      m_eulerAngle.x = 89.0f;
    if (m_eulerAngle.x < -89.0f)
      m_eulerAngle.x = -89.0f;

    m_dirForward = (CalculateDir(m_eulerAngle));
    m_dirRight = glm::normalize(glm::cross(m_dirForward, WORLD_UP));
    m_dirUp = glm::normalize(glm::cross(m_dirRight, m_dirForward));

    //Print("Position: ", m_position);
    //Print("Rotation: ", m_eulerAngle);
    //Print("Front: ", m_dirForward);
    //Print("Up: ", m_dirUp);
    //Print("Right: ", m_dirRight);
  }

  //*******************************************
  // Static Method
  //*******************************************
  glm::mat4 CameraObject::CalculateViewMatrix(glm::vec3 position
    , glm::vec3 dir, glm::vec3 worldUp)
  {
    return glm::lookAt(position, position + dir, worldUp);
  }

  glm::mat4 CameraObject::CalculateProjectionMatrix(CameraType camtype
    , float size, float aspect, float near_, float far_)
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
        //float aspect = 1.0f / ((static_cast<float>(Window::GetWidth() / Window::GetHeight()) * 0.5f);
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

  glm::mat4 CameraObject::CalculateJitteredProjectionMatrix(const CameraObject& cam, glm::vec2& jitteredUV)
  {
    return FrustumJitter::GetJitteredProjection(cam, jitteredUV);
  }

  void CameraObject::ProcessCameraInput(CameraObject& camera, float dt)
  {
    using namespace Input;
    static float moveSpeed = 4.0f;
    static const float walkSpeed = 4.0f;
    static const float runSpeed = 10.0f;

    static float mouseSpeed = 10.0f;
    static float rotateSpeed = 50.0f;

    if (Input::GetMouseHold(MouseKeyCode::MOUSE_BUTTON_RIGHT))
    {
      if (Input::GetKeyHold(KeyCode::KEY_LEFT_SHIFT))
      {
        moveSpeed = runSpeed;
      }
      else
      {
        moveSpeed = walkSpeed;
      }
      if (Input::GetKeyHold(KeyCode::KEY_W))
      {
        camera.Move(glm::vec3(0.0f, 0.0f, moveSpeed * dt));
      }
      if (Input::GetKeyHold(KeyCode::KEY_S))
      {
        camera.Move(glm::vec3(0.0f, 0.0f, -moveSpeed * dt));
      }
      if (Input::GetKeyHold(KeyCode::KEY_D))
      {
        camera.Move(glm::vec3(moveSpeed * dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_A))
      {
        camera.Move(glm::vec3(-moveSpeed * dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_Q))
      {
        camera.Move(glm::vec3(0.0f, -moveSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_E))
      {
        camera.Move(glm::vec3(0.0f, moveSpeed * dt, 0.0f));
      }

      if (Input::GetKeyHold(KeyCode::KEY_LEFT))
      {
        camera.Rotate(glm::vec3(0.0f, -rotateSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_RIGHT))
      {
        camera.Rotate(glm::vec3(0.0f, rotateSpeed * dt, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_UP))
      {
        camera.Rotate(glm::vec3(rotateSpeed * dt, 0.0f, 0.0f));
      }
      if (Input::GetKeyHold(KeyCode::KEY_DOWN))
      {
        camera.Rotate(glm::vec3(-rotateSpeed * dt, 0.0f, 0.0f));
      }

      {
        glm::vec2 offset = Input::GetMouseOffset();
        float pitch = -offset.y * mouseSpeed * dt;
        float yaw = offset.x * mouseSpeed * dt;
        camera.Rotate(glm::vec3(pitch
          , yaw, 0.0f));
      }
    }
  }

  float CameraObject::GetScreenAspectRatio(void)
  {
    return SCREEN_ASPECT_RATIO;
  }
}