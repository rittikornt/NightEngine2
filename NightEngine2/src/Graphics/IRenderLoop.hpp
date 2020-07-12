/*!
  @file IRenderLoop.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of IRenderLoop
*/
#pragma once

namespace Rendering
{
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
    COUNT
  };

  enum class DebugShadowView
  {
    NONE = 0,
    SHADOW_ONLY,
    SHADOW_CASCADE,
    COUNT
  };

  extern const char* k_debugViewStr[];
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
  protected:
    DebugView m_debugView = DebugView::NONE;
    DebugShadowView m_debugShadowView = DebugShadowView::NONE;
  };
} // Rendering

