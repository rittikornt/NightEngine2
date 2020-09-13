/*!
  @file PostProcessSetting.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessSetting
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/Material.hpp"

#include <glm/vec2.hpp>
#include <vector>

//Postprocess
#include "Graphics/Opengl/Postprocess/PostProcessUtility.hpp"
#include "Graphics/Opengl/Postprocess/SSAO.hpp"
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/Postprocess/FXAA.hpp"
#include "Graphics/Opengl/Postprocess/TAA.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

namespace Rendering
{
  class VertexArrayObject;
  class FrameBufferObject;

  namespace Postprocess
  {
    struct PostProcessContext
    {
      CameraObject*      camera;
      GBuffer*           gbuffer;

      FrameBufferObject* sceneFBO;
      VertexArrayObject* screenVAO;
      Texture*           screenTexture;

      float               time = 0.0f;
      float               screenZoomScale = 1.0f;
    };

    //! @brief PostProcessSetting struct
    struct PostProcessSetting
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER(PostProcessSetting, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(PostProcessSetting, m_resolution, true)
          .MR_ADD_MEMBER_PROTECTED(PostProcessSetting, m_bloomPP, true)
          .MR_ADD_MEMBER_PROTECTED(PostProcessSetting, m_ssaoPP, true)
          .MR_ADD_MEMBER_PROTECTED(PostProcessSetting, m_fxaaPP, true);
      }

      glm::ivec2        m_resolution = glm::ivec2(1);

      Material            m_uberPostMaterial;
      Material            m_blitCopyMaterial;

      //PostProcess
      Postprocess::PostProcessUtility   m_ppUtility;
      Postprocess::Bloom  m_bloomPP;
      Postprocess::SSAO   m_ssaoPP;
      Postprocess::FXAA   m_fxaaPP;
      Postprocess::TAA    m_taaPP;

      //Meta datas
      std::vector<PostProcessEffect*> m_postProcessEffects;

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply Postfx to the screen texture
      void Apply(const PostProcessContext& context);

      //! @brief Clear Color on fbo texture
      void Clear(void);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);

      //! @brief Get Effects Meta data for editor
      inline std::vector<PostProcessEffect*>& GetEffectsMetas(void) { return m_postProcessEffects;  }
    };
  }
}