/*!
  @file MurmurHash2.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of MurmurHash2
*/

#pragma once
#include "Core/Container/PrimitiveType.hpp"

#define MURMURHASH2(TYPE) NightEngine::Container::ConvertToHash(#TYPE, sizeof(#TYPE)-1)

namespace NightEngine
{
  namespace Container
  {
    constexpr U64 Murmur2A64_Hash(const char *str, unsigned long size, U64 seed)
    {
      constexpr U64 prime{0xc6a4a7935bd1e995ull};
      constexpr U32 shift1{19};
      constexpr U32 shift2{37};
      U64 hash{seed ^ (size * prime)};
      const U64 *data{(const U64 *)str};
      const U64 *end{data + (size / 8)};

      while (data != end)
      {
        U64 word{*data++};
        word *= prime;
        word ^= word >> shift1;
        word *= prime;
        hash ^= word;
        hash *= prime;
      }

      const unsigned char *byte_data{(const unsigned char *)data};

      switch (size & 7)
      {
      case 7:
        hash ^= ((U64)byte_data[6]) << 48;
      case 6:
        hash ^= ((U64)byte_data[5]) << 40;
      case 5:
        hash ^= ((U64)byte_data[4]) << 32;
      case 4:
        hash ^= ((U64)byte_data[3]) << 24;
      case 3:
        hash ^= ((U64)byte_data[2]) << 16;
      case 2:
        hash ^= ((U64)byte_data[1]) << 8;
      case 1:
        hash ^= ((U64)byte_data[0]);
      }

      hash ^= hash >> shift1;
      hash *= prime;
      hash ^= hash >> shift2;

      return hash;
    }

    constexpr U64 ConvertToHash(const char *str, size_t size)
    {
      constexpr U64 seed_prime{0xFFFFFFFFFFFFFFC5ull};
      return Murmur2A64_Hash(str
        , static_cast<unsigned long>(size)
        , seed_prime);
    }
    
  } // namespace Container
} // namespace NightEngine