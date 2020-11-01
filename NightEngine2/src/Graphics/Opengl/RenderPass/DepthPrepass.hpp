/*!
  @file DepthPrepass.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of DepthPrepass
*/
#pragma once
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/Material.hpp"

namespace NightEngine::Rendering::Opengl
{
  class Texture;
  struct GBuffer;
  class CameraObject;

  //! @brief DepthPrepass struct
  struct DepthPrepass
  {
    Material            m_depthPrepassMaterial;
    FrameBufferObject   m_fbo;
    int                 m_width  = 1;
    int                 m_height = 1;

    //! @brief Initialize DepthPrepass
    void Init(GBuffer& gbuffer);

    //! @brief Bind to fbo
    void Execute(CameraObject& camera);

    //! @brief Set the Texture binding units
    void RefreshTextureUniforms();
  };
}