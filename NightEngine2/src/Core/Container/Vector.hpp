/*!
  @file Vector.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Vector
*/

#pragma once
//! @brief for swapping to EASTL later on?
#include <vector>

namespace Core
{
  namespace Container
  {
    template<typename T>
    using Vector = std::vector<T>;
  }
}