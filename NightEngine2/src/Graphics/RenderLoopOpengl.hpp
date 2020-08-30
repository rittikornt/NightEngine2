/*!
  @file GraphicCore.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderLoopOpengl
*/
#pragma once

#include "Graphics/IRenderLoop.hpp"
#include "Core/EC/Handle.hpp"

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

  protected:
    void Render(void);

    void DrawScene(bool debugNormal);

    void DrawDebugIcons(void);

    void SetDeferredLightingPassUniforms(Material& material);

  public:
    float ambientStrength = 0.3f;
    float cameraFOV = 100.0f;
    float cameraFarPlane = 100.0f;
    
    float mainShadowsSize = 10.0f;
    float mainShadowsFarPlane = 100.0f;
    int mainShadowscascadeCount = 1;
    ShadowsResolution mainShadowResolution = ShadowsResolution::_2048;
    ShadowsResolution pointShadowResolution = ShadowsResolution::_1024;

  protected:
    //Uniform Buffer Object
    UniformBufferObject m_uniformBufferObject;

    //Depth FrameBuffer for Directional Shadow
    FrameBufferObject   m_depthDirShadowFBO;
    Texture             m_shadowMapDirShadowTexture;
    Material            m_depthDirShadowMaterial;

    //Depth FrameBuffer for Point Shadow
    FrameBufferObject   m_depthPointShadowFBO[POINTLIGHT_AMOUNT];
    Cubemap             m_shadowMapPointShadow[POINTLIGHT_AMOUNT];
    Material            m_depthPointShadowMaterial;

    glm::vec2           m_initResolution{1.0f};

    //Scene FrameBuffer
    GBuffer             m_gbuffer;
    FrameBufferObject   m_sceneFbo;
    Texture             m_sceneTexture;
    RenderBufferObject  m_sceneRbo;

    //PostProcess
    Rendering::Postprocess::PostProcessSetting* m_postProcessSetting;

    //FullScreen Postfx
    VertexArrayObject                  m_screenTriangleVAO;
    Material                           m_uberPostMaterial;
    Material                           m_blitCopyMaterial;
                                       
    //Cubemap IBL                      
    IBL                                m_ibl;

    //Material
    NightEngine::EC::Handle<Material>  m_defaultMaterial;
    Material                           m_lightingMaterial;

    Material                           m_debugViewMaterial;
    Material                           m_normalDebug;

    //Light
    NightEngine::EC::Handle<Material>  m_billboardMaterial;
    Texture                            m_lightTexture;
  };
} // Rendering

