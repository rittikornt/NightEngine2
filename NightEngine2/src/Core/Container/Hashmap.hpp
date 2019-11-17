/*!
  @file Hashmap.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Hashmap
*/

#pragma once
//! @brief for swapping to EASTL later on?
#include <unordered_map>

namespace Core
{
  namespace Container
  {
    template<typename KEY, typename VALUE>
    using Hashmap = std::unordered_map<KEY, VALUE>;

    template<typename KEY, typename VALUE, typename HASH>
    using HashmapCustom = std::unordered_map<KEY, VALUE, HASH>;
  }
}