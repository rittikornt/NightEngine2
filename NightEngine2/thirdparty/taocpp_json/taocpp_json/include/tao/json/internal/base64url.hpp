// Copyright (c) 2017-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_INTERNAL_BASE64URL_HPP
#define TAO_JSON_INTERNAL_BASE64URL_HPP

#include <stdexcept>
#include <string>

namespace tao
{
   namespace json
   {
      namespace internal
      {
         template< typename T >
         std::string base64url( const T& v )
         {
            static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

            std::string s;
            s.reserve( ( v.size() + 2 ) / 3 * 4 );

            unsigned cycle = 0;
            unsigned encode = 0;
            for( const auto c : v ) {
               encode <<= 8;
               encode += static_cast< unsigned char >( c );
               s += table[ ( encode >> ( ++cycle * 2 ) ) & 0x3f ];  // NOLINT
               if( cycle == 3 ) {
                  cycle = 0;
                  s += table[ encode & 0x3f ];  // NOLINT
               }
            }

            switch( cycle ) {
               case 0:
                  break;

               case 1:
                  s += table[ ( encode << 4 ) & 0x3f ];  // NOLINT
                  break;

               case 2:
                  s += table[ ( encode << 2 ) & 0x3f ];  // NOLINT
                  break;

               default:
                  throw std::logic_error( "code should be unreachable" );  // NOLINT, LCOV_EXCL_LINE
            }

            return s;
         }

      }  // namespace internal

   }  // namespace json

}  // namespace tao

#endif
