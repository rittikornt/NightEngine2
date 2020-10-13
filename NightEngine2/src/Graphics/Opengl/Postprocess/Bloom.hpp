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

namespace NightEngine::Rendering::Opengl
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
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_upscalingPass, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_scattering, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_bloomThreshold, true)
          .MR_ADD_MEMBER_PROTECTED(Bloom, m_blurIteration, true);
      }
      //Members
      FrameBufferObject m_targetFbo;
      Texture           m_targetTexture;

      FrameBufferObject m_bloomDownscaleFbo[k_bloomPyramidCount + 1];
      Texture           m_bloomDownscaleTexture[k_bloomPyramidCount + 1];

      FrameBufferObject m_bloomUpscaleFbo[k_bloomPyramidCount + 1];
      Texture           m_bloomUpscaleTexture[k_bloomPyramidCount + 1];

      glm::ivec2        m_resolution = glm::ivec2(1);;

      //Shader
      Shader            m_thresholdShader;
      Shader            m_blitCopyShader;
      Shader            m_bloomShader;
      Shader            m_upscalingShader;

      //Settings
      bool              m_useKawaseBlur = false;
      float             m_intensity = 1.0f;
      bool              m_halfResolution = false;
      bool              m_upscalingPass = true;
      float             m_scattering = 0.5f;
      float             m_bloomThreshold = 2.0f;
      int               m_blurIteration = 4;
      glm::vec2         m_blurDir = glm::vec2(0.0f);

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