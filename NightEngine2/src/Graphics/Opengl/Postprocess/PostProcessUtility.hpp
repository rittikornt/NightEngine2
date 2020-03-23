/*!
  @file PostProcessUtility.hpp
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
    struct PostProcessUtility
    {
      glm::ivec2        m_resolution;

      FrameBufferObject m_temp1Fbo;
      FrameBufferObject m_temp2Fbo;
      Texture           m_target1Texture;
      Texture           m_target2Texture;

      Shader            m_blitCopyShader;
      Shader            m_blurShader;
      Shader            m_kawaseBlurShader;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Clear Color on fbo texture
      void Clear(void);
      
      //! @brief Blur the target Texture
      void BlurTarget(Texture& target
        , VertexArrayObject& screenVAO
        , glm::ivec2 resolution, int iteration, bool useKawase = false);

      //! @brief Refresh Texture Uniforms
      void RefreshTextureUniforms(void);
    };
  }
}