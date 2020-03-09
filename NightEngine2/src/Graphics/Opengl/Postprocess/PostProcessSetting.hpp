/*!
  @file PostProcessSetting.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessSetting
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"

#include <glm/vec2.hpp>

//Postprocess
#include "Graphics/Opengl/Postprocess/SSAO.hpp"
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/Postprocess/FXAA.hpp"

namespace Rendering
{
  class VertexArrayObject;

  namespace Postprocess
  {
    struct PostProcessContext
    {
      CameraObject*      camera;
      GBuffer*           gbuffer;

      VertexArrayObject* screenVAO;
      Texture*           screenTexture;
    };

    //! @brief PostProcessSetting struct
    struct PostProcessSetting
    {
      glm::ivec2        m_resolution;

      //PostProcess
      Postprocess::Bloom  m_bloomPP;
      Postprocess::SSAO   m_ssaoPP;
      Postprocess::FXAA   m_fxaaPP;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply Postfx to the screen texture
      void Apply(const PostProcessContext& context);

      //! @brief Clear Color on fbo texture
      void Clear(void);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}