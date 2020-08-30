/*!
  @file GBuffer.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of GBuffer
*/
#pragma once
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/RenderBufferObject.hpp"
#include "Graphics/Opengl/Texture.hpp"

namespace Rendering
{
  class Shader;

  enum class GBufferTarget: size_t
  {
    PositionAndNormalX = 0,        // (0) vec4(pos.xyz, n.x)
    AlbedoAndMetallic,             // (1) vec4(albedo.xyz, metallic)
    LightSpacePositionAndNormalY,  // (2) vec4(lightSpacePos, n.y)
    EmissiveAndRoughness,          // (3) vec4(emissive.xyz, roughness)
    Count
  };
  //TODO: Calculate Position from Depth Buffer

  //! @brief GBuffer struct
  struct GBuffer
  {
    FrameBufferObject   m_fbo;
    Texture             m_textures[static_cast<size_t>(GBufferTarget::Count)];
    RenderBufferObject  m_depthBuffer;
    int                 m_width  = 1;
    int                 m_height = 1;

    //! @brief Initialize G buffer
    void Init(int width, int height);

    //! @brief Bind to fbo
    void Bind(void);

    //! @brief Unbind fbo
    void Unbind(void);

    //! @brief Bind all textures to its corresponding Textureunit
    void BindTextures(void);

    //! @brief Get Texture
    Texture& GetTexture(GBufferTarget target);

    //! @brief Get Texture
    Texture& GetTexture(size_t index);

    //! @brief Copy Depth buffer to fbo target (0 for screen fbo)
    void CopyDepthBufferTo(unsigned fboId);

    //! @brief Set the Texture binding units
    void RefreshTextureUniforms(Shader& shader);
  };
}