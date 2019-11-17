// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_EVENTS_STATISTICS_HPP
#define TAO_JSON_EVENTS_STATISTICS_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

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
         struct statistics
         {
            std::size_t null_count = 0;
            std::size_t true_count = 0;
            std::size_t false_count = 0;

            std::size_t signed_count = 0;
            std::size_t unsigned_count = 0;
            std::size_t double_count = 0;

            std::size_t string_count = 0;
            std::size_t string_lengths = 0;
            std::size_t key_count = 0;
            std::size_t key_lengths = 0;
            std::size_t binary_count = 0;
            std::size_t binary_lengths = 0;

            std::size_t local_date_count = 0;
            std::size_t local_time_count = 0;
            std::size_t local_date_time_count = 0;
            std::size_t offset_date_time_count = 0;

            std::size_t array_count = 0;
            std::size_t array_elements = 0;
            std::size_t object_count = 0;
            std::size_t object_members = 0;

            void null() noexcept
            {
               ++null_count;
            }

            void boolean( const bool v ) noexcept
            {
               ++( v ? true_count : false_count );
            }

            void number( const std::int64_t /*unused*/ ) noexcept
            {
               ++signed_count;
            }

            void number( const std::uint64_t /*unused*/ ) noexcept
            {
               ++unsigned_count;
            }

            void number( const double /*unused*/ ) noexcept
            {
               ++double_count;
            }

            void string( const tao::string_view v ) noexcept
            {
               ++string_count;
               string_lengths += v.size();
            }

            void binary( const tao::binary_view v ) noexcept
            {
               ++binary_count;
               binary_lengths += v.size();
            }

            void local_date( const json::local_date /*unused*/ ) noexcept
            {
               ++local_date_count;
            }

            void local_time( const json::local_time /*unused*/ ) noexcept
            {
               ++local_time_count;
            }

            void local_date_time( const json::local_date_time /*unused*/ ) noexcept
            {
               ++local_date_time_count;
            }

            void offset_date_time( const json::offset_date_time /*unused*/ ) noexcept
            {
               ++offset_date_time_count;
            }

            void begin_array( const std::size_t /*unused*/ = 0 ) noexcept
            {
               ++array_count;
            }

            void element() noexcept
            {
               ++array_elements;
            }

            void end_array( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }

            void begin_object( const std::size_t /*unused*/ = 0 ) noexcept
            {
               ++object_count;
            }

            void key( const tao::string_view v ) noexcept
            {
               ++key_count;
               key_lengths += v.size();
            }

            void member() noexcept
            {
               ++object_members;
            }

            void end_object( const std::size_t /*unused*/ = 0 ) noexcept
            {
            }
         };

      }  // namespace events

   }  // namespace json

}  // namespace tao

#endif
