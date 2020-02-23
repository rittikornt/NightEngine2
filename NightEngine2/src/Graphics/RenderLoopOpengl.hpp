/*!
  @file GraphicCore.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderLoopOpengl
*/
#pragma once

#include "Graphics/IRenderLoop.hpp"

//FrameBuffer Test
#include "Graphics/Opengl/FrameBufferObject.hpp"
#include "Graphics/Opengl/RenderBufferObject.hpp"

//Graphics
#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Vertex.hpp"
#include "Graphics/Opengl/Material.hpp"
#include "Graphics/Opengl/Cubemap.hpp"
#include "Graphics/Opengl/UniformBufferObject.hpp"
#include "Graphics/Opengl/GBuffer.hpp"
#include "Graphics/Opengl/IBL.hpp"

//Postprocess
#include "Graphics/Opengl/Postprocess/SSAO.hpp"
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/Postprocess/FXAA.hpp"

#define POINTLIGHT_AMOUNT 4
#define SPOTLIGHT_AMOUNT 4

namespace Rendering
{
  class RenderLoopOpengl: public IRenderLoop
  {
  public:
    virtual void Initialize(void) override;

    //TODO: remove float dt later, only here for debuging camera
    virtual void Render(float dt) override;

    virtual void Terminate(void) override;
    
    virtual void OnRecompiledShader(void) override;
  private:
    void Render(void);

    void DrawScene(bool debugNormal);

    //Uniform Buffer Object
    UniformBufferObject g_uniformBufferObject;

    //Depth FrameBuffer for Directional Shadow
    FrameBufferObject   g_depthfbo;
    Texture             g_shadowMapTexture;
    Material            g_depthMaterial;

    //Depth FrameBuffer for Point Shadow
    FrameBufferObject   g_depth2fbo[POINTLIGHT_AMOUNT];
    Cubemap             g_shadowMapCubemap[POINTLIGHT_AMOUNT];
    Material            g_depth2Material;

    glm::vec2           g_initResolution;
    float               g_shadowWidth, g_shadowHeight;

    //Scene FrameBuffer
    GBuffer             m_gbuffer;
    FrameBufferObject   m_sceneFbo;
    Texture             m_sceneTexture;
    RenderBufferObject  m_sceneRbo;

    //PostProcess
    Postprocess::Bloom  g_bloomPP;
    Postprocess::SSAO   g_ssaoPP;
    Postprocess::FXAA   g_fxaaPP;

    //Screen Quad
    VertexArrayObject   g_screenVAO;
    Material            g_screenMaterial;
    Material            g_screenQuadMaterial;

    //Cubemap IBL
    IBL                 g_ibl;

    //Material
    Material*           g_defaultMaterial;
    Material            g_lightingMaterial;
    Material            g_normalDebug;

    //Light
    Material             g_billboardMaterial;
    Texture              g_lightTexture;
  };
} // Rendering

