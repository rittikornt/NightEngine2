/*!
  @file Map.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Map
*/

#pragma once
  //! @brief for swapping to EASTL later on?
  #include <map>

namespace NightEngine
{
  namespace Container
  {
    template<typename KEY, typename VALUE>
    using Map = std::map<KEY, VALUE>;
  }
}