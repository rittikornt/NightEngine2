/*!
  @file IRenderLoop.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of IRenderLoop
*/
#pragma once

namespace Rendering
{
  enum class ShadowsResolution
  {
    _512 = 512,
    _1024 = 1024,
    _2048 = 2048,
    _4096 = 4096
  };
  extern const ShadowsResolution k_shadowResEnum[4];

  enum class GraphicsAPI
  {
    OPENGL = 0,
    VULKAN,
    DX12
  };

  enum class DebugView
  {
    NONE = 0,
    ALBEDO,
    NORMAL_WORLD_SPACE,
    METALLIC,
    ROUGHNESS,
    EMISSIVE,
    LIGHT_SPACE_POS,
    AMBIENT,
    MAIN_SHADOW_DEPTH,
    COUNT
  };
  extern const char* k_debugViewStr[];

  enum class DebugShadowView
  {
    NONE = 0,
    SHADOW_MAIN_ONLY,
    SHADOW_ALL,
    SHADOW_CASCADE,
    COUNT
  };
  extern const char* k_debugShadowViewStr[];

  class IRenderLoop
  {
  public:
    virtual ~IRenderLoop() {}

    virtual void Initialize(void) {}

    virtual void Terminate(void) {}

    //TODO: remove float dt later, only here for debuging camera
    virtual void Render(float dt) = 0;

    virtual void OnRecompiledShader(void) {}

    inline void SetDebugViews(DebugView dv, DebugShadowView dsv) { m_debugView = dv; m_debugShadowView = dsv;}

    inline bool IsDebugView(void) { return m_debugView != DebugView::NONE || m_debugShadowView != DebugShadowView::NONE; }

    inline GraphicsAPI GetAPI(void) { return m_graphicsAPI; }
  protected:
    GraphicsAPI m_graphicsAPI = GraphicsAPI::OPENGL;

    DebugView m_debugView = DebugView::NONE;
    DebugShadowView m_debugShadowView = DebugShadowView::NONE;
  };
} // Rendering

