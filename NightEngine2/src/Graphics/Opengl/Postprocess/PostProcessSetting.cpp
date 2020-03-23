/*!
  @file PostProcessSetting.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of PostProcessSetting
*/
#include "Graphics/Opengl/Postprocess/PostProcessSetting.hpp"

#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/GBuffer.hpp"

#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"

namespace Rendering
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(PostProcessSetting)

    void PostProcessSetting::Init(int width, int height)
    {
      m_ppUtility.Init(width, height);

      //Bloom
      m_bloomPP.Init(width, height);

      //SSAO
      m_ssaoPP.Init(width, height);

      //FXAA
      m_fxaaPP.Init(width, height);

      m_postProcessEffects.emplace_back(&m_bloomPP);
      m_postProcessEffects.emplace_back(&m_ssaoPP);
      m_postProcessEffects.emplace_back(&m_fxaaPP);
    }

    void PostProcessSetting::Apply(const PostProcessContext& context)
    {
      //SSAO
      if (m_ssaoPP.m_enable)
      {
        m_ssaoPP.Apply(*(context.screenVAO), *(context.camera), *(context.gbuffer));
      }
      else
      {
        m_ssaoPP.Clear();
      }

      //Bloom
      if (m_bloomPP.m_enable)
      {
        m_bloomPP.Apply(*(context.screenVAO), *(context.screenTexture), m_ppUtility);
      }
      else
      {
        m_bloomPP.Clear();
      }
    }

    void PostProcessSetting::Clear(void)
    {
      m_ppUtility.Clear();

      m_ssaoPP.Clear();
      m_bloomPP.Clear();
    }

    void PostProcessSetting::RefreshTextureUniforms(void)
    {
      m_ppUtility.RefreshTextureUniforms();

      //Postprocessing
      m_bloomPP.RefreshTextureUniforms();
      m_ssaoPP.RefreshTextureUniforms();
      m_fxaaPP.RefreshTextureUniforms();
    }
  }
}