/*!
  @file FXAA.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FXAA
*/

#pragma once
#include "Graphic/Shader.hpp"

#include <glm/vec2.hpp>

namespace Graphic
{
  class FrameBufferObject;
  class VertexArrayObject;
  class Texture;

  namespace Postprocess
  {
    //! @brief FXAA struct
    struct FXAA
    {
      Shader    m_fxaaShader;
      glm::vec2 m_resolution;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply FXAA to the screen texture
      void Apply(VertexArrayObject& screenQuad
        , Texture& screenTexture
        , FrameBufferObject& sceneFbo);

      //! @brief Apply FXAA to the screen directly
      void ApplyToScreen(VertexArrayObject& screenQuad
        , Texture& screenTexture);
    };
  }
}