/*!
  @file TestComponent.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of TestComponent
*/

#include "Core/EC/Components/TestComponent.hpp"

namespace NightEngine
{
  namespace EC
  {
    namespace Components
    {
      INIT_REFLECTION_AND_FACTORY(Controller, 10, 5)

      INIT_REFLECTION_AND_FACTORY(CharacterInfo, 10, 5)

      INIT_REFLECTION_AND_FACTORY(CTimer, 10, 5)
    }
  }
}