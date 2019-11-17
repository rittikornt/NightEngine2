// Copyright (c) 2017-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_JAXN_EVENTS_TO_STREAM_HPP
#define TAO_JSON_JAXN_EVENTS_TO_STREAM_HPP

#include "../../events/to_stream.hpp"

#include "../../internal/hexdump.hpp"

#include "../../local_date.hpp"
#include "../../local_date_time.hpp"
#include "../../local_time.hpp"
#include "../../offset_date_time.hpp"

#include "../is_identifier.hpp"

namespace tao
{
   namespace json
   {
      namespace jaxn
      {
         namespace events
         {
            // Events consumer to build a JAXN string representation.

            struct to_stream
               : public json::events::to_stream
            {
               using json::events::to_stream::number;
               using json::events::to_stream::to_stream;

               void number( const double v )
               {
                  next();
                  if( !std::isfinite( v ) ) {
                     if( std::isnan( v ) ) {  // NOLINT
                        os << "NaN";
                     }
                     else if( v < 0 ) {
                        os << "-Infinity";
                     }
                     else {
                        os << "Infinity";
                     }
                  }
                  else {
                     json_double_conversion::Dtostr( os, v );
                  }
               }

               void key( const tao::string_view v )
               {
                  if( json::jaxn::is_identifier( v ) ) {
                     next();
                     os.write( v.data(), v.size() );
                  }
                  else {
                     string( v );
                  }
                  os.put( ':' );
                  first = true;
               }

               void binary( const tao::binary_view v )
               {
                  next();
                  os.put( '$' );
                  json::internal::hexdump( os, v );
               }

               void local_date( const json::local_date v )
               {
                  next();
                  os << v;
               }

               void local_time( const json::local_time v )
               {
                  next();
                  os << v;
               }

               void local_date_time( const json::local_date_time v )
               {
                  next();
                  os << v;
               }

               void offset_date_time( const json::offset_date_time v )
               {
                  next();
                  os << v;
               }
            };

         }  // namespace events

      }  // namespace jaxn

   }  // namespace json

}  // namespace tao

#endif
