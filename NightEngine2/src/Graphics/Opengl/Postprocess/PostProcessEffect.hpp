/*!
  @file PostProcessEffect.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessEffect
*/

#pragma once

#define INIT_POSTPROCESSEFFECT() m_metaType = METATYPE_FROM_OBJECT(*this)

namespace NightEngine::Rendering::Opengl
{
  namespace Postprocess
  {
    struct PostProcessEffect
    {
      NightEngine::Reflection::MetaType* m_metaType = nullptr;
      bool m_enable = true;

      void GetVariable(NightEngine::Reflection::Variable& variable)
      {
        variable = NightEngine::Reflection::Variable{ m_metaType, this};
      }
    };
  }
}