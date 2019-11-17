// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_INTERNAL_ARRAY_TRAITS_HPP
#define TAO_JSON_INTERNAL_ARRAY_TRAITS_HPP

#include <algorithm>

#include "../forward.hpp"
#include "../type.hpp"

#include "../events/produce.hpp"

namespace tao
{
   namespace json
   {
      namespace internal
      {
         template< typename T >
         struct array_traits
         {
            template< template< typename... > class Traits >
            static bool is_nothing( const T& o )
            {
               return o.empty();
            }

            template< template< typename... > class Traits, typename Base >
            static void assign( basic_value< Traits, Base >& v, const T& o )
            {
               v.prepare_array();
               for( const auto& i : o ) {
                  v.unsafe_emplace_back( i );
               }
            }

            template< template< typename... > class Traits, typename Consumer >
            static void produce( Consumer& c, const T& o )
            {
               c.begin_array( o.size() );
               for( const auto& i : o ) {
                  json::events::produce< Traits >( c, i );
                  c.element();
               }
               c.end_array( o.size() );
            }

            template< template< typename... > class Traits, typename Base >
            static bool equal( const basic_value< Traits, Base >& lhs, const T& rhs ) noexcept
            {
               const auto* p = lhs.skip_value_ptr();
               return p->is_array() && ( p->unsafe_get_array().size() == rhs.size() ) && std::equal( rhs.begin(), rhs.end(), p->unsafe_get_array().begin() );
            }

            template< template< typename... > class Traits, typename Base >
            static bool less_than( const basic_value< Traits, Base >& lhs, const T& rhs ) noexcept
            {
               const auto* p = lhs.skip_value_ptr();
               return p->is_array() ? std::lexicographical_compare( p->unsafe_get_array().begin(), p->unsafe_get_array().end(), rhs.begin(), rhs.end() ) : ( p->type() < type::ARRAY );
            }

            template< template< typename... > class Traits, typename Base >
            static bool greater_than( const basic_value< Traits, Base >& lhs, const T& rhs ) noexcept
            {
               const auto* p = lhs.skip_value_ptr();
               return p->is_array() ? std::lexicographical_compare( rhs.begin(), rhs.end(), p->unsafe_get_array().begin(), p->unsafe_get_array().end() ) : ( p->type() > type::ARRAY );
            }
         };

      }  // namespace internal

   }  // namespace json

}  // namespace tao

#endif
