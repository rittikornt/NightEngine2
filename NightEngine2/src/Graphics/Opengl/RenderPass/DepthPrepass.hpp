/*!
  @file DepthPrepass.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of DepthPrepass
*/
#pragma once
#include "Graphics/Opengl/FrameBufferObject.hpp"

namespace Rendering
{
  class Texture;

  //! @brief DepthPrepass struct
  struct DepthPrepass
  {
    FrameBufferObject   m_fbo;
    int                 m_width  = 1;
    int                 m_height = 1;

    //! @brief Initialize DepthPrepass
    void Init(int width, int height, const Texture& depthTexture);

    //! @brief Bind to fbo
    void Bind(void);

    //! @brief Unbind fbo
    void Unbind(void);

    //! @brief Set the Texture binding units
    void RefreshTextureUniforms();
  };
}