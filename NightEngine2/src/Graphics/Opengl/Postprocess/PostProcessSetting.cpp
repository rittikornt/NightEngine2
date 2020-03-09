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

namespace Rendering
{
  namespace Postprocess
  {
    void PostProcessSetting::Init(int width, int height)
    {
      //Bloom
      m_bloomPP.Init(width, height);

      //SSAO
      m_ssaoPP.Init(width, height);

      //FXAA
      m_fxaaPP.Init(width, height);
    }

    void PostProcessSetting::Apply(const PostProcessContext& context)
    {
      //SSAO
      m_ssaoPP.Apply(*(context.screenVAO), *(context.camera), *(context.gbuffer));

      //Bloom
      m_bloomPP.Apply(*(context.screenVAO), *(context.screenTexture));
    }

    void PostProcessSetting::Clear(void)
    {
      m_ssaoPP.Clear();
      m_bloomPP.Clear();
    }

    void PostProcessSetting::RefreshTextureUniforms(void)
    {
      //Postprocessing
      m_bloomPP.RefreshTextureUniforms();
      m_ssaoPP.RefreshTextureUniforms();
      m_fxaaPP.RefreshTextureUniforms();
    }
  }
}