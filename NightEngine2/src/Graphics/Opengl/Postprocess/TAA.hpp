/*!
  @file TAA.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of TAA
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include <glm/vec2.hpp>

namespace NightEngine::Rendering::Opengl
{
  class FrameBufferObject;
  class VertexArrayObject;
  struct GBuffer;
  struct CameraObject;

  namespace Postprocess
  {
    //! @brief TAA struct
    struct TAA : public PostProcessEffect
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER_WITHBASE(TAA, PostProcessEffect
          , NightEngine::Reflection::BaseClass::InheritType::PUBLIC, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(TAA, m_TAAShader, true)
          .MR_ADD_MEMBER_PROTECTED(TAA, m_sharpness, true)
          .MR_ADD_MEMBER_PROTECTED(TAA, m_frustumJitterStrength, true)
          .MR_ADD_MEMBER_PROTECTED(TAA, m_beforeTonemapping, true);
      }

      Shader    m_TAAShader;

      Texture   m_currRT;
      Texture   m_historyRT;

      FrameBufferObject m_taaFBO;
      FrameBufferObject m_copyHistoryFBO;

      float m_sharpness = 1.0f;
      bool m_beforeTonemapping = true;
      float m_frustumJitterStrength = 1.0f;

      bool m_isFirstFrame = true;
      glm::ivec2 m_resolution = glm::ivec2(0);

      //! @brief Initialization
      void LazyInit(int width, int height);

      //! @brief Apply TAA to the screen texture
      void Apply(VertexArrayObject& screenVAO
        , GBuffer& gbuffer, Texture& screenTexture
        , FrameBufferObject& sceneFbo, const CameraObject& cam);

      //! @brief Apply TAA to the screen directly
      void ApplyToScreen(VertexArrayObject& screenVAO
        , Texture& screenTexture);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}