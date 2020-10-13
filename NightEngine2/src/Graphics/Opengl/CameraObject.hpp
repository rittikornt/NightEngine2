/*!
  @file CameraObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of CameraObject
*/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEFAULT_CAM_POS (glm::vec3(0.0f, 0.0f, 10.0f))
#define DEFAULT_FORWARD (glm::vec3(0.0f, 0.0f, -1.0f))
#define DEFAULT_ANGLE (glm::vec3(0.0f, -90.0f, 0.0f))

#define WORLD_UP (glm::vec3(0.0f, 1.0f, 0.0f))
#define VEC3_ZERO (glm::vec3(0.0f, 0.0f, 0.0f))

namespace NightEngine::Rendering::Opengl
{
  class Shader;
  class Cubemap;

	struct CameraObject
	{
    enum class CameraType: unsigned
    {
      PERSPECTIVE = 0,
      ORTHOGRAPHIC
    };

    CameraType m_projectionType = CameraType::PERSPECTIVE;
		glm::vec3 m_position = DEFAULT_CAM_POS;
		glm::vec3 m_eulerAngle = VEC3_ZERO;

		//Dir
		glm::vec3 m_dirForward;
		glm::vec3 m_dirRight;
		glm::vec3 m_dirUp;

		//Matrix
		glm::mat4 m_view;
		glm::mat4 m_projection;
    glm::mat4 m_VP;

    glm::mat4 m_invView;
    glm::mat4 m_invProjection;
    glm::mat4 m_invVP;

    //TAA
    bool m_bJitterProjectionMatrix = false;  //Use when TAA is enabled
    int m_taaFrameIndex = 0;
    float m_jitterStrength = 0.5f;
    glm::vec2 m_activeJitteredUV;

    //MotionVector
    glm::mat4 m_unjitteredProjection;
    glm::mat4 m_unjitteredVP;
    glm::mat4 m_prevUnjitteredVP;

    union CameraSize
    {
      float m_fov = 90.0f;
      float m_size;
      CameraSize(void) = default;
      CameraSize(float size) 
        :m_fov(size){}
    } m_camSize;

    float m_near = 0.3f;
    float m_far = 100.0f;

    //////////////////////////////////////////////////////////

    //! @brief Constructor
    CameraObject();

    //! @brief Constructor
    CameraObject(CameraType type, float size, glm::vec3 pos = DEFAULT_CAM_POS
      , float near_ = 0.3f, float far_ = 100.0f);

    //////////////////////////////////////////////////////////

    void ApplyCameraInfo(Shader& shader);

    void ApplyUnJitteredProjectionMatrix(Shader& shader);

    void ApplyViewMatrix(Shader& shader);

    void ApplyViewMatrix(Cubemap& cubemap);

    //////////////////////////////////////////////////////////

    void OnStartFrame(void);

    void OnEndFrame(void);

    //////////////////////////////////////////////////////////

    glm::mat4& GetUnjitteredProjectionMatrix(void);

    glm::mat4& GetViewMatix(void);

    //! @brief Translate camera based on its world direction
    void Move(glm::vec3 amount);

    //! @brief Translate camera with absolute position
    void Translate(glm::vec3 pos);

    glm::vec3 CalculateDir(glm::vec3 rotation);

    void Rotate(glm::vec3 rotateAmount);

    //*******************************************
    // Static Method
    //*******************************************
    static glm::mat4 CalculateViewMatrix(glm::vec3 position
      , glm::vec3 dir, glm::vec3 worldUp = WORLD_UP);

    static glm::mat4 CalculateProjectionMatrix(CameraType camtype
      , float size, float aspect, float near_, float far_);

    static glm::mat4 CalculateJitteredProjectionMatrix(const CameraObject& cam
      , glm::vec2& jitteredUV, float jitterStrength);

    static void ProcessCameraInput(CameraObject& camera, float dt);

    static float GetScreenAspectRatio(void);

    static glm::i32vec2 GetScreenSize(void);
	};
}