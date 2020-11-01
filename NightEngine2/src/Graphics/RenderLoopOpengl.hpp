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
#include "Graphics/Opengl/IBL.hpp"

//Render Passes
#include "Graphics/Opengl/RenderPass/DepthPrepass.hpp"
#include "Graphics/Opengl/RenderPass/GBuffer.hpp"

//Postprocess
#include "Graphics/Opengl/Postprocess/SSAO.hpp"
#include "Graphics/Opengl/Postprocess/Bloom.hpp"
#include "Graphics/Opengl/Postprocess/FXAA.hpp"
#include "Graphics/Opengl/RenderPass/CameraMotionVector.hpp"

#define POINTLIGHT_AMOUNT 4
#define SPOTLIGHT_AMOUNT 4

namespace NightEngine::Rendering::Opengl
{
  struct SceneBuffer
  {
    Opengl::FrameBufferObject   m_sceneFbo;
    Opengl::Texture             m_sceneTexture;
    Opengl::VertexArrayObject   m_screenTriangleVAO;
    glm::ivec2 m_resolution = glm::ivec2(0, 0);

    void LazyInit(CameraObject& camera, GBuffer& gbuffer);
  };
}

namespace NightEngine::Rendering
{
  namespace Opengl::Postprocess
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

    void DrawDebugIcons(void);

    void SetDeferredLightingPassUniforms(Opengl::Material& material);

  public:
    float screenZoomScale = 1.0f;

    float ambientStrength = 0.3f;
    float cameraFOV = 100.0f;
    float cameraFarPlane = 100.0f;
    
    float mainShadowsSize = 10.0f;
    float mainShadowsFarPlane = 100.0f;
    int mainShadowscascadeCount = 1;
    ShadowsResolution mainShadowResolution = ShadowsResolution::_2048;
    ShadowsResolution pointShadowResolution = ShadowsResolution::_1024;

    //int m_width = 1;
    //int m_height = 1;

  protected:
    Opengl::SceneBuffer m_sceneBuffer;

    //Uniform Buffer Object
    Opengl::UniformBufferObject m_uniformBufferObject;

    //Depth FrameBuffer for Directional Shadow
    Opengl::FrameBufferObject   m_depthDirShadowFBO;
    Opengl::Texture             m_shadowMapDirShadowTexture;
    Opengl::Material            m_depthDirShadowMaterial;

    //Depth FrameBuffer for Point Shadow
    Opengl::FrameBufferObject   m_depthPointShadowFBO[POINTLIGHT_AMOUNT];
    Opengl::Cubemap             m_shadowMapPointShadow[POINTLIGHT_AMOUNT];
    Opengl::Material            m_depthPointShadowMaterial;

    //glm::vec2           m_initResolution{1.0f};

    //Render Passes
    Opengl::DepthPrepass        m_depthPrepass;
    Opengl::GBuffer             m_gbuffer;
    NightEngine::Rendering::Opengl::Prepass::CameraMotionVector m_cameraMotionVector;

    //PostProcess
    NightEngine::Rendering::Opengl::Postprocess::PostProcessSetting* m_postProcessSetting;

    //Cubemap IBL                      
    Opengl::IBL                                m_ibl;

    //Material
    NightEngine::EC::Handle<Opengl::Material>  m_defaultMaterial;
    Opengl::Material                           m_lightingMaterial;

    Opengl::Material                           m_debugViewMaterial;

    //Light
    NightEngine::EC::Handle<Opengl::Material>  m_billboardMaterial;
    Opengl::Texture                            m_lightTexture;
  };
} // Rendering

