/*!
  @file PostProcessEffect.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PostProcessEffect
*/

#pragma once

#define INIT_POSTPROCESSEFFECT() m_metaType = METATYPE_FROM_OBJECT(*this)

namespace Rendering
{
  namespace Postprocess
  {
    struct PostProcessEffect
    {
      //NightEngine::Reflection::Variable m_variable;
      NightEngine::Reflection::MetaType* m_metaType;
      bool m_enable = true;

      void GetVariable(NightEngine::Reflection::Variable& variable)
      {
        variable = NightEngine::Reflection::Variable{ m_metaType, this};
      }
    };
  }
}