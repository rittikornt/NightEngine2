/*!
  @file IBL.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of IBL
*/
#include "Graphic/IBL.hpp"

#include "Graphic/PrimitiveShape.hpp"
#include "Core/Serialization/FileSystem.hpp"
#include "Graphic/CameraObject.hpp"

using namespace Graphic::PrimitiveShape;
using namespace Core;

namespace Graphic
{
  //******************************************
  // Capture Projection, View Matrices
  //******************************************
  static glm::mat4 g_captureProjection = CameraObject::CalculateProjectionMatrix(CameraObject::CameraType::PERSPECTIVE
    , 90.0f, 1.0f, 0.1f, 10.0f);
  static glm::mat4 g_captureViews[] =
  {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
  };

  //Resolution
  const glm::ivec2 g_irrandianceMapResolution{ 256,256 };
  const glm::ivec2 g_prefilteredMapResolution{ 512,512 };
  const glm::ivec2 g_brdfLUTResolution{ 512, 512 };

  void IBL::Init(CameraObject& camera, VertexArrayObject& screenVAO)
  {
    m_width = 512, m_height = 512;

    //Cube VAO
    m_cubeVAO.Init();
    m_cubeVAO.Build(BufferMode::Static, Cube::cubeVertices
      , Cube::cubeIndices, Cube::info);

    //Cubemaps
    m_cubemap.Init(m_width, m_height, "skybox.vert"
      , "skybox.frag", Texture::Channel::RGB16F);
    camera.ApplyProjectionMatrix(m_cubemap.GetShader());

    m_irradianceCubemap.Init(g_irrandianceMapResolution.x, g_irrandianceMapResolution.y
      , "skybox.vert", "skybox.frag"
      , Texture::Channel::RGB16F);
    camera.ApplyProjectionMatrix(m_irradianceCubemap.GetShader());

    //Specular Cubemap
    m_prefilterMap.Init(g_prefilteredMapResolution.x, g_prefilteredMapResolution.y
      , "IBL/cubemap_debug.vert", "IBL/cubemap_debug.frag"
      , Texture::Channel::RGB16F
      , Texture::FilterMode::TRILINEAR, Texture::FilterMode::LINEAR
      , true);
    camera.ApplyProjectionMatrix(m_prefilterMap.GetShader());

    //Specular Shader
    m_prefilterShader.Init();
    m_prefilterShader.AttachShaderFile("IBL/cubemap_prefilteredmap.vert");
    m_prefilterShader.AttachShaderFile("IBL/cubemap_prefilteredmap.frag");
    m_prefilterShader.Link();

    m_brdfShader.Init();
    m_brdfShader.AttachShaderFile("IBL/precompute_brdflut.vert");
    m_brdfShader.AttachShaderFile("IBL/precompute_brdflut.frag");
    m_brdfShader.Link();
    
    //HDR to Cubemap Shader
    m_cubeConversionShader.Init();
    m_cubeConversionShader.AttachShaderFile("IBL/equirectangular_to_cubemap.vert");
    m_cubeConversionShader.AttachShaderFile("IBL/equirectangular_to_cubemap.frag");
    m_cubeConversionShader.Link();

    //Irradiancemap Shader
    m_irradianceShader.Init();
    m_irradianceShader.AttachShaderFile("IBL/cubemap_irradiancemap.vert");
    m_irradianceShader.AttachShaderFile("IBL/cubemap_irradiancemap.frag");
    m_irradianceShader.Link();

    //Capture FBO
    m_captureFBO.Init();
    m_captureRBO.Init(m_width, m_height
      , RenderBufferObject::Format::DEPTH24);
    m_captureFBO.AttachRenderBuffer(m_captureRBO);

    //Baking IBL
    //TODO: Don't bake every startup
    ConvertHDRToCubemap("HDRI/Walk_Of_Fame/Mans_Outside_2k.hdr");
    BakeIrradiancemap(g_irrandianceMapResolution);
    BakePrefilteredmap(g_prefilteredMapResolution);
    BakeBRDFLUT(g_brdfLUTResolution, screenVAO);
  }

  void IBL::ConvertHDRToCubemap(std::string fileName)
  {
    //Texture
    m_hdriTexture = Texture(FileSystem::GetFilePath(fileName, FileSystem::DirectoryType::Cubemaps)
      , Texture::Channel::RGB16F
      , Texture::FilterMode::LINEAR
      , Texture::WrapMode::CLAMP_TO_EDGE, true);

    //******************************************
    // Convert HDR texture to Cubemap
    //******************************************
    glDisable(GL_CULL_FACE);

    //Capture
    glViewport(0, 0, m_width, m_height);
    m_captureFBO.Bind();
    {
      m_cubeConversionShader.Bind();
      {
        //Texture
        m_cubeConversionShader.SetUniform("u_equirectangularMap", 0);
        m_hdriTexture.BindToTextureUnit(0);

        //Matrix
        m_cubeConversionShader.SetUniform("u_projection", g_captureProjection);

        //Render each face of the cube into the Cubemap
        for (unsigned i = 0; i < 6; ++i)
        {
          m_cubeConversionShader.SetUniform("u_view", g_captureViews[i]);

          //Render Target
          m_captureFBO.AttachCubemapFace(m_cubemap, 0, i);

          //Render
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          m_cubeVAO.Draw();
        }
      }
      m_cubeConversionShader.Unbind();
    }
    m_captureFBO.Unbind();

    glEnable(GL_CULL_FACE);
  }

  void IBL::BakeIrradiancemap(glm::ivec2 resolution)
  {
    //******************************************
    // Precompute irradiance map
    //******************************************
    //Scale RBO's resolution
    m_captureRBO.SetBuffer(resolution.x, resolution.y
      , RenderBufferObject::Format::DEPTH24);

    glDisable(GL_CULL_FACE);
    glViewport(0, 0, resolution.x, resolution.y);
    m_captureFBO.Bind();
    {
      m_irradianceShader.Bind();
      {
        m_irradianceShader.SetUniform("u_cubemap", 0);
        m_cubemap.BindToTextureUnit(0);

        //Matrix
        m_irradianceShader.SetUniform("u_projection", g_captureProjection);

        //Render each face of the cube into the Cubemap
        for (unsigned i = 0; i < 6; ++i)
        {
          m_irradianceShader.SetUniform("u_view", g_captureViews[i]);

          //Render Target
          m_captureFBO.AttachCubemapFace(m_irradianceCubemap, 0, i);

          //Render
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          m_cubeVAO.Draw();
        }
      }
      m_irradianceShader.Unbind();
    }
    m_captureFBO.Unbind();

    glEnable(GL_CULL_FACE);
  }

  void IBL::BakePrefilteredmap(glm::ivec2 resolution)
  {
    glDisable(GL_CULL_FACE);
    m_captureFBO.Bind();
    {
      m_prefilterShader.Bind();
      {
        m_prefilterShader.SetUniform("u_cubemap", 0);
        m_cubemap.BindToTextureUnit(0);

        //Matrix
        m_prefilterShader.SetUniform("u_projection", g_captureProjection);

        //Render for each mipmap levels
        const unsigned mipmapLevels = 5;
        for (unsigned m = 0; m < mipmapLevels; ++m)
        {
          auto sizeMultiplier = std::pow(0.5, m);
          unsigned width = resolution.x * sizeMultiplier;
          unsigned height = resolution.y * sizeMultiplier;

          //Scale RBO's resolution
          m_captureRBO.SetBuffer(width, height
            , RenderBufferObject::Format::DEPTH24);
          glViewport(0, 0, width, height);

          //Roughness
          float roughness = (float)m / (float)(mipmapLevels - 1);
          m_prefilterShader.SetUniform("u_roughness", roughness);

          //Render each face of the cube into the Cubemap
          for (unsigned i = 0; i < 6; ++i)
          {
            m_prefilterShader.SetUniform("u_view", g_captureViews[i]);

            //Render Target
            m_captureFBO.AttachCubemapFace(m_prefilterMap, 0, i, m);

            //Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_cubeVAO.Draw();
          }
        }
      }
      m_prefilterShader.Unbind();
    }
    m_captureFBO.Unbind();

    glEnable(GL_CULL_FACE);
  }

  void IBL::BakeBRDFLUT(glm::ivec2 resolution
    , VertexArrayObject& screenVAO)
  {
    m_brdfLUT = Texture::GenerateNullTexture(512, 512
      , Texture::Channel::RG16F, Texture::Channel::RG
      , Texture::FilterMode::LINEAR, Texture::WrapMode::CLAMP_TO_EDGE);

    m_captureRBO.SetBuffer(resolution.x, resolution.y
      , RenderBufferObject::Format::DEPTH24);
    //Render Target
    m_captureFBO.AttachTexture(m_brdfLUT, 0);

    glViewport(0, 0, resolution.x, resolution.y);
    m_captureFBO.Bind();
    {
      m_brdfShader.Bind();
      {
        //Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        screenVAO.Draw();
      }
      m_brdfShader.Unbind();
    }
    m_captureFBO.Unbind();
  }

  void IBL::DrawCubemap(CubemapType type, CameraObject camera)
  {
    switch (type)
    {
      case CubemapType::NORMAL:
      {
        camera.ApplyViewMatrix(m_cubemap);
        m_cubemap.Draw();
        break;
      }
      case CubemapType::IRRADIANCE:
      {
        camera.ApplyViewMatrix(m_irradianceCubemap);
        m_irradianceCubemap.Draw();
        break;
      }
      case CubemapType::PREFILTERED:
      {
        camera.ApplyViewMatrix(m_prefilterMap);
        m_prefilterMap.Draw();
        break;
      }
    }

  }
}
