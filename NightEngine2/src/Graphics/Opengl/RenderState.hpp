/*!
  @file RenderState.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderState
*/
#pragma once

namespace NightEngine::Rendering::Opengl
{
  enum RenderFeature : int
  {
    OBJECT_VELOCITY = 1 << 0,
    GBUFFER_MASK = 1 << 1,
    FEATURE_2 = 1 << 2,
    FEATURE_3 = 1 << 3,
    FEATURE_4 = 1 << 4,
    FEATURE_5 = 1 << 5,
    FEATURE_6 = 1 << 6,
    FEATURE_7 = 1 << 7
  };

  namespace RenderSetup
  {
    //! @brief Always write refBits to stencil
    void WriteStencilAlways(int refBits);

    //! @brief Only draw pixel that has none of the specifics bit set
    void PassStencilIfBitNotSet(int refBits);

    //! @brief Only draw pixel that has specifics bit set
    void PassStencilIfBitSet(int refBits);
  }
}