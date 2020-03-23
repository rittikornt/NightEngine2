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
  namespace Postprocess
  {
    class PostProcessSetting;
  }

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
    UniformBufferObject m_uniformBufferObject;

    //Depth FrameBuffer for Directional Shadow
    FrameBufferObject   m_depthfbo;
    Texture             m_shadowMapTexture;
    Material            m_depthMaterial;

    //Depth FrameBuffer for Point Shadow
    FrameBufferObject   m_depth2fbo[POINTLIGHT_AMOUNT];
    Cubemap             m_shadowMapCubemap[POINTLIGHT_AMOUNT];
    Material            m_depth2Material;

    glm::vec2           m_initResolution;
    float               m_shadowWidth, m_shadowHeight;

    //Scene FrameBuffer
    GBuffer             m_gbuffer;
    FrameBufferObject   m_sceneFbo;
    Texture             m_sceneTexture;
    RenderBufferObject  m_sceneRbo;

    //PostProcess
    Rendering::Postprocess::PostProcessSetting* m_postProcessSetting;

    //FullScreen Postfx
    VertexArrayObject   m_screenTriangleVAO;
    Material            m_postfxFinalMaterial;
    Material            m_blitCopyMaterial;

    //Cubemap IBL
    IBL                 m_ibl;

    //Material
    Material*           m_defaultMaterial;
    Material            m_lightingMaterial;
    Material            m_normalDebug;

    //Light
    Material*            m_billboardMaterial;
    Texture              m_lightTexture;
  };
} // Rendering

