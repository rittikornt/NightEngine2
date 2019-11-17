// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_CONSUME_HPP
#define TAO_JSON_CONSUME_HPP

#include <type_traits>

#include "forward.hpp"

#include "internal/type_traits.hpp"

namespace tao
{
   namespace json
   {
      template< typename T, template< typename... > class Traits = traits, typename Producer >
      typename std::enable_if< internal::has_consume_one< Traits, Producer, T >::value, T >::type consume( Producer& parser )
      {
         return Traits< T >::template consume< Traits >( parser );
      }

      template< typename T, template< typename... > class Traits = traits, typename Producer >
      typename std::enable_if< !internal::has_consume_one< Traits, Producer, T >::value, T >::type consume( Producer& parser )
      {
         T t;
         Traits< T >::template consume< Traits >( parser, t );
         return t;
      }

      template< template< typename... > class Traits = traits, typename Producer, typename T >
      typename std::enable_if< internal::has_consume_two< Traits, Producer, T >::value, void >::type consume( Producer& parser, T& t )
      {
         Traits< T >::template consume< Traits >( parser, t );
      }

      template< template< typename... > class Traits = traits, typename Producer, typename T >
      typename std::enable_if< !internal::has_consume_two< Traits, Producer, T >::value, void >::type consume( Producer& parser, T& t )
      {
         t = Traits< T >::template consume< Traits >( parser );
      }

   }  // namespace json

}  // namespace tao

#endif
