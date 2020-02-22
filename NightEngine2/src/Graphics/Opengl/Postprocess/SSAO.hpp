/*!
  @file SSAO.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of SSAO
*/

#pragma once
#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/FrameBufferObject.hpp"

#include <vector>
#include <glm/vec3.hpp>

namespace Rendering
{
  class VertexArrayObject;
  class GBuffer;
  class CameraObject;

  namespace Postprocess
  {
    //! @brief SSAO struct
    struct SSAO
    {
      FrameBufferObject m_fbo;
      Texture           m_ssaoTexture;
      Shader            m_ssaoShader;
      Shader            m_simpleBlur;

      //Settings
      int               m_intensity = 1;
      glm::vec3         m_color{1.0f};
      float             m_sampleRadius = 0.5f;
      float             m_bias = 0.025f;

      //Sample Kernel 
      std::vector<glm::vec3> m_sampleKernel;
      std::vector<std::string> m_sampleKernelString;
      unsigned m_sampleAmount = 64;

      Texture m_noiseTexture; //For rotating the sample kernel

      //! @brief Initialization
      void Init(int width, int height);

      //! @brief Apply SSAO to the screen texture
      void Apply(VertexArrayObject& screenQuad
        ,CameraObject& camera, GBuffer& gbuffer);

      //! @brief Clear Color on fbo texture
      void Clear(void);

      //! @brief Refresh states
      void RefreshTextureUniforms(void);

      //! @brief Lerp
      inline float Lerp(float a, float b, float f)
      {
        return a + f * (b - a);
      }
    };
  }
}