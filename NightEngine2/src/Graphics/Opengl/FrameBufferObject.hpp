/*!
  @file FrameBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FrameBufferObject
*/
#pragma once
#include <vector>

namespace NightEngine::Rendering::Opengl
{
  //Forward declaration
  class Texture;
  struct TextureIdentifier;
  class RenderBufferObject;
  class Cubemap;

  class FrameBufferObject
  {
    public:
      //! @brief Constructor
      FrameBufferObject(void):m_id(~(0)) {}
      
      //! @brief Destructor
      ~FrameBufferObject(void);

      //! @brief Get ID
      unsigned GetID(void) { return m_id; }

      //! @brief Initialize the Frame Buffer Object
      void Init(void);

      //! @brief Bind
      void Bind(void);

      //! @brief Unbind
      void Unbind(void);

      //! @brief Special Drawcall to multiple render target
      void SetupMultipleRenderTarget(void);

      //! @brief Attach texture to framebuffer
      void AttachCubemap(Cubemap& cubemap);

      //! @brief Attach Cubemap face texture to framebuffer
      void AttachCubemapFace(Cubemap& cubemap
        , int textureIndex = 0, int cubemapIndex = 0
        , int mipmapLevel = 0);

      //! @brief Attach color texture to framebuffer
      void AttachColorTexture(const Texture& texture, int textureIndex = 0);

      //! @brief Attach depth texture to framebuffer
      void AttachDepthTexture(const Texture& texture);

      //! @brief Attach depth/stencil texture to framebuffer
      TextureIdentifier CreateAndAttachDepthStencilTexture(float width, float height
        , int minFilter, int maxFilter);

      //! @brief Attach depth texture to framebuffer
      TextureIdentifier CreateAndAttachDepthTexture(int width, int height);

      //! @brief Attach Render buffer to framebuffer
      void AttachRenderBuffer(RenderBufferObject& rbo);

      //! @brief Copy buffer to the target FBO
      void CopyBufferToTarget(int width, int height
        , int targetWidth, int targetHeight
        , unsigned fboId, unsigned bitField);

      //! @brief Copy buffer to the target Texture
      void CopyToTexture(Texture& target
        ,int newWidth, int newHeight);
    private:
      //! @brief Bind
      void BindUnsafe(void);

      unsigned int          m_id;
      std::vector<unsigned> m_renderTarget;
  };

}