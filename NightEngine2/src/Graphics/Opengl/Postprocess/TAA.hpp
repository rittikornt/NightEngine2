/*!
  @file TAA.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of TAA
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include <glm/vec2.hpp>

#include "Graphics/Opengl/Texture.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"

namespace Rendering
{
  class FrameBufferObject;
  class VertexArrayObject;
  class Texture;
  class GBuffer;
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
          .MR_ADD_MEMBER_PROTECTED(TAA, m_TAAShader, true);
      }

      Shader    m_TAAShader;
      Texture   m_historyRT;
      int m_width = 1;
      int m_height = 1;

      //! @brief Initialization
      void Init(int width, int height);

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