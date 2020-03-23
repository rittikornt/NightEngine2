/*!
  @file SSAO.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of SSAO
*/

#include "Graphics/Opengl/Postprocess/SSAO.hpp"

#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/CameraObject.hpp"
#include "Graphics/Opengl/GBuffer.hpp"

#include <random>

namespace Rendering
{
  namespace Postprocess
  {
    INIT_REFLECTION_FOR(SSAO)

    void SSAO::Init(int width, int height)
    {
      INIT_POSTPROCESSEFFECT();
      m_ssaoTexture = Texture::GenerateNullTexture(
        width, height, Texture::Channel::RGB
        , Texture::Channel::RGB, Texture::FilterMode::NEAREST
        , Texture::WrapMode::CLAMP_TO_EDGE);
      
      //FBO
      m_fbo.Init();
      m_fbo.AttachTexture(m_ssaoTexture);
      m_fbo.Bind();
      m_fbo.Unbind();

      //Shader
      m_ssaoShader.Create();
      m_ssaoShader.AttachShaderFile("Postprocess/ssao.vert");
      m_ssaoShader.AttachShaderFile("Postprocess/ssao.frag");
      m_ssaoShader.Link();

      m_simpleBlur.Create();
      m_simpleBlur.AttachShaderFile("Postprocess/simple_blur.vert");
      m_simpleBlur.AttachShaderFile("Postprocess/simple_blur.frag");
      m_simpleBlur.Link();

      //Set Uniform
      RefreshTextureUniforms();

      //Sample kernel
      std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
      std::default_random_engine generator;
      for (unsigned i = 0; i < m_sampleAmount; ++i)
      {
        glm::vec3 sampleDir{
          randomFloats(generator) * 2.0 - 1.0,
          randomFloats(generator) * 2.0 - 1.0,
          randomFloats(generator) };

        //Random Hemisphere offset
        sampleDir = glm::normalize(sampleDir);
        float magnitude = randomFloats(generator);

        //Scale by magnitude
        float scale = (float)i / 64.0;
        scale = Lerp(0.1f, 1.0f, scale * scale);
        magnitude *= scale;

        m_sampleKernel.emplace_back(sampleDir * magnitude);
        m_sampleKernelString.emplace_back("u_sampleKernel["
          + std::to_string(i) + "]");
      }

      //Noise texture
      std::vector<glm::vec3> ssaoNoise;
      for (unsigned i = 0; i < 16; ++i)
      {
        glm::vec3 noise{ randomFloats(generator)
          , randomFloats(generator)
          , 0.0f };
        ssaoNoise.emplace_back(noise);
      }
      m_noiseTexture = Texture::GenerateTextureData(&ssaoNoise[0]
        , 4, 4, Texture::Channel::RGB16F, Texture::Channel::RGB
        , Texture::FilterMode::NEAREST, Texture::WrapMode::REPEAT);
    }

    void SSAO::Apply(VertexArrayObject& screenVAO
      , CameraObject& camera, GBuffer& gbuffer)
    {
      //Generate AO
      m_fbo.Bind();
      {
        camera.ApplyViewMatrix(m_ssaoShader);
        camera.ApplyProjectionMatrix(m_ssaoShader);

        m_ssaoShader.Bind();
        {
          gbuffer.GetTexture(0).BindToTextureUnit(0);
          gbuffer.GetTexture(1).BindToTextureUnit(1);

          //Noise and sample kernel
          m_noiseTexture.BindToTextureUnit(2);
          for (int i = 0; i < m_sampleAmount; ++i)
          {
            m_ssaoShader.SetUniform(m_sampleKernelString[i], m_sampleKernel[i]);
          }

          //Uniforms
          m_ssaoShader.SetUniform("u_intensity", m_intensity);
          m_ssaoShader.SetUniform("u_ssaoColor", m_color);
          m_ssaoShader.SetUniform("u_sampleRadius", m_sampleRadius);
          m_ssaoShader.SetUniform("u_bias", m_bias);

          screenVAO.Draw();
        }
        m_ssaoShader.Unbind();
      }
      m_fbo.Unbind();

      //Blur the AO
      m_fbo.Bind();
      {
        m_simpleBlur.Bind();
        {
          m_ssaoTexture.BindToTextureUnit(0);

          screenVAO.Draw();
        }
        m_simpleBlur.Unbind();
      }
      m_fbo.Unbind();

    }

    void SSAO::Clear(void)
    {
      m_fbo.Bind();
      {
        //Clear with white
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
      }
      m_fbo.Unbind();
    }

    void SSAO::RefreshTextureUniforms(void)
    {
      //Set Uniform
      m_ssaoShader.Bind();
      {
        m_ssaoShader.SetUniform("u_gbufferResult.m_positionTex", 0);
        m_ssaoShader.SetUniform("u_gbufferResult.m_normalTex", 1);
        m_ssaoShader.SetUniform("u_noiseTexture", 2);
      }
      m_ssaoShader.Unbind();

      m_simpleBlur.Bind();
      {
        m_simpleBlur.SetUniform("u_screenTexture", 0);
      }
      m_simpleBlur.Unbind();
    }
  }
}