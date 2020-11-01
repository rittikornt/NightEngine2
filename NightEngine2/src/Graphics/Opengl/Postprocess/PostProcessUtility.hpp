/*!
  @file PostProcessUtility.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessUtility
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace NightEngine::Rendering::Opengl
{
  class VertexArrayObject;

  namespace Postprocess
  {
    //! @brief PostProcessUtility struct
    struct PostProcessUtility
    {
      glm::ivec2        m_resolution = glm::ivec2(0);
      glm::vec4         m_clearColor = glm::vec4(0.0f);

      FrameBufferObject m_temp1Fbo;
      FrameBufferObject m_temp2Fbo;
      Texture           m_target1Texture;
      Texture           m_target2Texture;

      Shader            m_blitCopyShader;
      Shader            m_blurShader;
      Shader            m_kawaseBlurShader;

      //! @brief Initialization
      void LazyInit(int width, int height);

      //! @brief Clear Color on fbo texture
      void Clear(void);
      
      //! @brief Blur the target Texture
      void BlurTarget(glm::vec4 clearColor,Texture& target, VertexArrayObject& screenVAO
        , glm::ivec2 resolution, int iteration, bool useKawase = false);

      //! @brief Blur the target Texture
      void BlurTarget(glm::vec4 clearColor, FrameBufferObject& targetFbo, Texture& target, VertexArrayObject& screenVAO
        , glm::ivec2 resolution, int iteration, bool useKawase = false);

      //! @brief Refresh Texture Uniforms
      void RefreshTextureUniforms(void);
    };
  }
}