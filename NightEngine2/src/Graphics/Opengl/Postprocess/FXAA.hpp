/*!
  @file FXAA.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FXAA
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include <glm/vec2.hpp>

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"

namespace Rendering
{
  class FrameBufferObject;
  class VertexArrayObject;
  class Texture;

  namespace Postprocess
  {
    //! @brief FXAA struct
    struct FXAA : public PostProcessEffect
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER_WITHBASE(FXAA, PostProcessEffect
          , NightEngine::Reflection::BaseClass::InheritType::PUBLIC, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(FXAA, m_fxaaShader, true);
      }

      Shader    m_fxaaShader;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply FXAA to the screen texture
      void Apply(VertexArrayObject& screenVAO
        , Texture& screenTexture
        , FrameBufferObject& sceneFbo);

      //! @brief Apply FXAA to the screen directly
      void ApplyToScreen(VertexArrayObject& screenVAO
        , Texture& screenTexture);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}