// Copyright (c) 2016-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_EVENTS_DISCARD_HPP
#define TAO_JSON_EVENTS_DISCARD_HPP

#include <cstddef>
#include <cstdint>
#include <string>

#include "../binary_view.hpp"
#include "../local_date.hpp"
#include "../local_date_time.hpp"
#include "../local_time.hpp"
#include "../offset_date_time.hpp"

#include "../external/string_view.hpp"

namespace tao
{
   namespace json
   {
      namespace events
      {
         // Events consumer that discards events.

         struct discard
         {
            void null() noexcept
            {
            }

            void boolean( const bool /*unused*/ ) noexcept
            {
            }

            void number( const std::int64_t /*unused*/ ) noexcept
            {
            }

            void number( const std::uint64_t /*unused*/ ) noexcept
            {
            }

            void number( const double /*unused*/ ) noexcept
            {
            }

            void string( const tao::string_view /*unused*/ ) noexcept
            {
            }

            void binary( const tao::binary_view /*unused*/ ) noexcept
            {
            }

            void local_date( const json::local_date /*unused*/ ) noexcept
            {
            }

            void local_time( const json::local_time /*unused*/ ) noexcept
            {
            }

            void local_date_time( const json::local_date_time /*unused*/ ) noexcept
            {
            }

            void offset_date_time( const json::offset_date_time /*unused*/ ) noexcept
            {
            }

            void begin_array( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }

            void element() noexcept
            {
            }

            void end_array( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }

            void begin_object( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }

            void key( const tao::string_view /*unused*/ ) noexcept
            {
            }

            void member() noexcept
            {
            }

            void end_object( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }
         };

      }  // namespace events

   }  // namespace json

}  // namespace tao

#endif
