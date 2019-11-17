// Copyright (c) 2017-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_UBJSON_EVENTS_TO_STREAM_HPP
#define TAO_JSON_UBJSON_EVENTS_TO_STREAM_HPP

#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>

#include "../../binary_view.hpp"
#include "../../local_date.hpp"
#include "../../local_date_time.hpp"
#include "../../local_time.hpp"
#include "../../offset_date_time.hpp"

#include "../../external/string_view.hpp"

#include "../../internal/endian.hpp"

namespace tao
{
   namespace json
   {
      namespace ubjson
      {
         namespace events
         {
            class to_stream
            {
            private:
               std::ostream& os;

            public:
               explicit to_stream( std::ostream& in_os ) noexcept
                  : os( in_os )
               {
               }

               void null()
               {
                  os.put( 'Z' );
               }

               void boolean( const bool v )
               {
                  os.put( v ? 'T' : 'F' );
               }

               void number( const std::int64_t v )
               {
                  if( ( v >= -128 ) && ( v <= 127 ) ) {
                     os.put( 'i' );
                     const auto x = static_cast< std::int8_t >( v );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else if( ( v >= -32768 ) && ( v <= 32767 ) ) {
                     os.put( 'I' );
                     const std::uint16_t x = json::internal::h_to_be( std::uint16_t( v ) );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else if( ( v >= -2147483648ll ) && ( v <= 2147483647ll ) ) {
                     os.put( 'l' );
                     const std::uint32_t x = json::internal::h_to_be( std::uint32_t( v ) );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else {
                     os.put( 'L' );
                     const std::uint64_t x = json::internal::h_to_be( std::uint64_t( v ) );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
               }

               void number( const std::uint64_t v )
               {
                  if( v <= 255 ) {
                     os.put( 'U' );
                     const auto x = static_cast< std::uint8_t >( v );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else if( v <= 32767 ) {
                     os.put( 'I' );
                     const std::uint16_t x = json::internal::h_to_be( std::uint16_t( v ) );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else if( v <= 2147483647ul ) {
                     os.put( 'l' );
                     const std::uint32_t x = json::internal::h_to_be( std::uint32_t( v ) );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else if( v <= 9223372036854775807ull ) {
                     os.put( 'L' );
                     const std::uint64_t x = json::internal::h_to_be( v );
                     os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
                  }
                  else {
                     os.put( 'H' );
                     os.put( 'U' );
                     os.put( char( 19 ) + char( v >= 10000000000000000000ull ) );
                     os << v;
                  }
               }

               void number( const double v )
               {
                  os.put( 'D' );
                  const double x = json::internal::h_to_be( v );
                  os.write( static_cast< const char* >( static_cast< const void* >( &x ) ), sizeof( x ) );
               }

               void string( const tao::string_view v )
               {
                  if( ( v.size() == 1 ) && ( ( v[ 0 ] & 0x80 ) == 0 ) ) {
                     os.put( 'C' );
                     os.put( v[ 0 ] );
                  }
                  else {
                     os.put( 'S' );
                     number( std::uint64_t( v.size() ) );
                     os.write( v.data(), v.size() );
                  }
               }

               void binary( const tao::binary_view v )
               {
                  // NOTE: UBJSON encodes binary data as 'strongly typed array of uint8 values'.
                  os.write( "[$U#", 4 );
                  number( std::uint64_t( v.size() ) );
                  os.write( static_cast< const char* >( static_cast< const void* >( v.data() ) ), v.size() );
               }

               void local_date( const json::local_date /*unused*/ )
               {
                  // if this throws, consider using local_date_to_* transformers
                  throw std::runtime_error( "local date invalid for UBJSON representation" );  // NOLINT
               }

               void local_time( const json::local_time /*unused*/ )
               {
                  // if this throws, consider using local_time_to_* transformers
                  throw std::runtime_error( "local time invalid for UBJSON representation" );  // NOLINT
               }

               void local_date_time( const json::local_date_time /*unused*/ )
               {
                  // if this throws, consider using local_date_time_to_* transformers
                  throw std::runtime_error( "local date-time invalid for UBJSON representation" );  // NOLINT
               }

               void offset_date_time( const json::offset_date_time /*unused*/ )
               {
                  // if this throws, consider using offset_date_time_to_* transformers
                  throw std::runtime_error( "offset date-time invalid for UBJSON representation" );  // NOLINT
               }

               void begin_array()
               {
                  os.put( '[' );
               }

               void begin_array( const std::size_t size )
               {
                  os.write( "[#", 2 );
                  number( std::uint64_t( size ) );
               }

               void element() noexcept
               {
               }

               void end_array()
               {
                  os.put( ']' );
               }

               void end_array( const std::size_t /*unused*/ ) noexcept
               {
               }

               void begin_object()
               {
                  os.put( '{' );
               }

               void begin_object( const std::size_t size )
               {
                  os.write( "{#", 2 );
                  number( std::uint64_t( size ) );
               }

               void key( const tao::string_view v )
               {
                  number( std::uint64_t( v.size() ) );
                  os.write( v.data(), v.size() );
               }

               void member() noexcept
               {
               }

               void end_object()
               {
                  os.put( '}' );
               }

               void end_object( const std::size_t /*unused*/ ) noexcept
               {
               }
            };

         }  // namespace events

      }  // namespace ubjson

   }  // namespace json

}  // namespace tao

#endif
