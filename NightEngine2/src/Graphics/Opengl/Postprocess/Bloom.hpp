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
#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"

namespace Rendering
{
  class VertexArrayObject;

  namespace Postprocess
  {
    struct PostProcessUtility;

    //Const
    const int k_bloomPyramidCount = 5;

    //! @brief Bloom struct
    struct Bloom: public PostProcessEffect
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER_WITHBASE(Bloom, PostProcessEffect
          , NightEngine::Reflection::BaseClass::InheritType::PUBLIC, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_useKawaseBlur, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_intensity, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_halfResolution, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_scattering, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_bloomThreshold, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_blurIteration, true);
      }
      //Members
      FrameBufferObject m_targetFbo;
      FrameBufferObject m_bloomFbo[k_bloomPyramidCount + 1];
      Texture           m_targetTexture;
      Texture           m_bloomTexture[k_bloomPyramidCount + 1];
      glm::ivec2        m_resolution;

      //Shader
      Shader            m_thresholdShader;
      Shader            m_blitCopyShader;
      Shader            m_bloomShader;

      //Settings
      bool              m_useKawaseBlur = false;
      float             m_intensity = 1.0f;
      bool              m_halfResolution = false;
      float             m_scattering = 0.5f;
      float             m_bloomThreshold = 6.0f;
      int               m_blurIteration = 4;
      glm::vec2         m_blurDir;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply Bloom to the screen texture
      void Apply(VertexArrayObject& screenVAO
        , Texture& screenTexture, PostProcessUtility& ppUtility);

      //! @brief Clear Color on fbo texture
      void Clear(void);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}