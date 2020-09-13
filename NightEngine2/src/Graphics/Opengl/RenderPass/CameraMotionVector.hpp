/*!
  @file FXAA.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FXAA
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"
#include "Graphics/Opengl/Postprocess/PostProcessEffect.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include <glm/vec2.hpp>

namespace Rendering
{
  class VertexArrayObject;
  class Texture;
  class GBuffer;
  class CameraObject;

  namespace Prepass
  {
    //! @brief CameraMotionVector struct
    struct CameraMotionVector : public Postprocess::PostProcessEffect
    {
      REFLECTABLE_TYPE_BLOCK()
      {
        META_REGISTERER_WITHBASE(CameraMotionVector, PostProcessEffect
          , NightEngine::Reflection::BaseClass::InheritType::PUBLIC, true
          , nullptr, nullptr)
          .MR_ADD_MEMBER_PROTECTED(CameraMotionVector, m_cmvShader, true);
      }

      FrameBufferObject m_fbo;
      Shader    m_cmvShader;
      bool m_isFirstFrame = true;

      //! @brief Initialization
      void Init(int width, int height, GBuffer& gbuffer);

      //! @brief Render CameraMotionVector
      void Render(VertexArrayObject& screenVAO
        , GBuffer& gbuffer, CameraObject& cam);

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);
    };
  }
}