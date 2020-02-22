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

namespace Rendering
{
  class VertexArrayObject;

  namespace Postprocess
  {
    //! @brief Bloom struct
    struct Bloom
    {
      FrameBufferObject m_bloomFbo;
      Texture           m_targetTexture;
      Texture           m_bloomTexture[5];
      glm::ivec2        m_resolution;

      //Shader
      Shader            m_thresholdShader;
      Shader            m_blurShader;
      Shader            m_bloomShader;

      //Settings
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