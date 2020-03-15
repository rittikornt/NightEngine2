/*!
  @file Bloom.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Bloom
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"

#include <glm/vec2.hpp>

#include "Core/Reflection/ReflectionMacros.hpp"

namespace Rendering
{
  class VertexArrayObject;

  namespace Postprocess
  {
    //! @brief Bloom struct
    struct Bloom
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER(Bloom, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_resolution, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_intensity, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_bloomThreshold, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_blurIteration, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_blurDir, true);
      }

      FrameBufferObject m_bloomFbo;
      Texture           m_targetTexture;
      Texture           m_bloomTexture[5];
      glm::ivec2        m_resolution;

      //Shader
      Shader            m_thresholdShader;
      Shader            m_blurShader;
      Shader            m_bloomShader;

      //Settings
      float             m_intensity = 0.2f;
      float             m_bloomThreshold = 4.0f;
      unsigned          m_blurIteration = 10;
      glm::vec2         m_blurDir;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply Bloom to the screen texture
      void Apply(VertexArrayObject& screenQuad
        , Texture& screenTexture);

      //! @brief Blur the target Texture
      void BlurTarget(Texture& target
        , VertexArrayObject& screenQuad
        , glm::ivec2 resolution);

      //! @brief Clear Color on fbo texture
      void Clear(void);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}