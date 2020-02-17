/*!
  @file IBL.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of IBL
*/

#pragma once
#include "Graphic/Opengl/FrameBufferObject.hpp"
#include "Graphic/Opengl/RenderBufferObject.hpp"
#include "Graphic/Opengl/Texture.hpp"
#include "Graphic/Opengl/Shader.hpp"
#include "Graphic/Opengl/VertexArrayObject.hpp"
#include "Graphic/Opengl/Cubemap.hpp"

namespace Graphic
{
  struct CameraObject;

  //! @brief IBL class
  struct IBL
  {
    enum class CubemapType
    {
      SKYBOX = 0,
      IRRADIANCE, //Diffuse
      PREFILTERED //Specular
    };

    int                 m_width, m_height;

    Cubemap             m_cubemap;
    VertexArrayObject   m_cubeVAO;

    //Diffuse
    Cubemap             m_irradianceCubemap;
    Shader              m_irradianceShader;

    Texture             m_brdfLUT;
    Shader              m_brdfShader;

    //Specular
    Cubemap             m_prefilterMap;
    Shader              m_prefilterShader;

    //Hdr texture to Cubemap
    Texture             m_hdriTexture;
    Shader              m_cubeConversionShader;
    FrameBufferObject   m_captureFBO;
    RenderBufferObject  m_captureRBO;

    //! brief Initialization
    void Init(CameraObject& camera, VertexArrayObject& screenVAO);

    //! brief Convert HDR file to Cubemap
    void ConvertHDRToCubemap(std::string fileName);

    //! brief Bake the irradiance into cubemap
    void BakeIrradiancemap(glm::ivec2 resolution);

    //! brief Prefiltered specular into cubemap
    void BakePrefilteredmap(glm::ivec2 resolution);

    //! brief Bake BRDF LUT into 2D texture
    void BakeBRDFLUT(glm::ivec2 resolution, VertexArrayObject& screenVAO);

    //! brief Draw cubemap
    void DrawCubemap(CubemapType type, CameraObject& camera);

    //! @brief Refresh states
    void RefreshTextureUniforms(CameraObject& camera);
  };
}