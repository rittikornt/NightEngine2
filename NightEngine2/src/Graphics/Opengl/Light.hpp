/*!
  @file Light.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Light
*/

#pragma once
#include <Graphics/Color.hpp>
#include "Graphics/Opengl/Shader.hpp"
#include "Core/EC/ComponentLogic.hpp"

#include <glm/mat4x4.hpp>

//TODO: maybe this class shouldn't be in Rendering namespace?
namespace NightEngine::Rendering::Opengl
{
  struct CameraObject;

  class Light : public NightEngine::EC::ComponentLogic
  {
    REFLECTABLE_TYPE_BLOCK()
    {
      META_REGISTERER_WITHBASE(Light, NightEngine::EC::ComponentLogic
        , InheritType::PUBLIC, true
        , nullptr, nullptr)
        .MR_ADD_MEMBER_PROTECTED(Light, m_lightType, true)
        .MR_ADD_MEMBER_PROTECTED(Light, m_lightInfo, true)
        .MR_ADD_MEMBER_PROTECTED(Light, m_lightIndex, true);
    }
    public:
      //! @brief Light information
      struct LightInfo
      {
        Color3 m_color;

        union Value
        {
          float m_intensity;
          struct SpotlightValue
          {
            float m_inner;
            float m_outer;
            float m_spotLightIntensity;
          } m_spotLight;

          Value(void) = default;
          Value(float intensity);
          Value(float innerAngle, float outerAngle, float intensity);
        } m_value;
      };

      enum class LightType: unsigned
      {
        DIRECTIONAL = 0,
        POINT,
        SPOTLIGHT
      };

      //! @brief Constructor
      Light(void) = default;

      //! @brief Constructor
      Light(LightType type, LightInfo info);

      //! @brief Initialze
      void Init(LightType type, LightInfo info, int lightIndex);

      //! @brief Apply Light uniform to shader
      void ApplyLightInfo(Shader& shader);

      //! @brief Get Light information
      inline const LightInfo& GetLightInfo(void) const { return m_lightInfo; }

      //! @brief Get Light information
      inline LightType GetLightType(void) { return m_lightType; }

      //! @brief Get Light information
      inline int GetLightIndex(void) { return m_lightIndex; }

      //! @brief Calculate Light Space Matrix for DirLight Shadow calculation
      glm::mat4& CalculateDirLightWorldToLightSpaceMatrix(CameraObject camera, float size
        ,float near_ = 0.01f, float far_ = 100.0f);

      //! @brief Calculate Light Space Matrix for 6 directions for Omni-directional Shadow
      std::vector<glm::mat4>& CalculatePointLightWorldToLightSpaceMatrices(float size = 90.0f
        , float aspect = 16.0f/9.0f
        , float near_ = 0.01f, float far_ = 100.0f);
    private:
      LightType m_lightType;
      LightInfo m_lightInfo;
      int       m_lightIndex;

      std::vector<glm::mat4> m_worldToLightSpaceMatrix; //For Shadow calculation
  };
}