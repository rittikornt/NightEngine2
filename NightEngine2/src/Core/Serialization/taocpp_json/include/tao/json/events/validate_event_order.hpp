// Copyright (c) 2016-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_EVENTS_VALIDATE_EVENT_ORDER_HPP
#define TAO_JSON_EVENTS_VALIDATE_EVENT_ORDER_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "../binary_view.hpp"
#include "../local_date.hpp"
#include "../local_date_time.hpp"
#include "../local_time.hpp"
#include "../offset_date_time.hpp"

#include "../external/byte.hpp"
#include "../external/optional.hpp"
#include "../external/string_view.hpp"

namespace tao
{
   namespace json
   {
      namespace events
      {
         // Events consumer that validates the order of events.

         class validate_event_order
         {
         private:
            enum state_t
            {
               EXPECT_TOP_LEVEL_VALUE,
               EXPECT_ARRAY_VALUE_OR_END,
               EXPECT_ARRAY_ELEMENT,
               EXPECT_SIZED_ARRAY_VALUE_OR_END,
               EXPECT_SIZED_ARRAY_ELEMENT,
               EXPECT_OBJECT_KEY_OR_END,
               EXPECT_OBJECT_VALUE,
               EXPECT_OBJECT_MEMBER,
               EXPECT_SIZED_OBJECT_KEY_OR_END,
               EXPECT_SIZED_OBJECT_VALUE,
               EXPECT_SIZED_OBJECT_MEMBER,
               EXPECT_NOTHING
            };

            struct sizes_t
            {
               explicit sizes_t( const std::size_t in_expected )
                  : expected( in_expected )
               {
               }

               void check( const std::size_t in_expected )
               {
                  if( expected != in_expected ) {
                     throw std::logic_error( "inconsistent size" );  // NOLINT
                  }
                  if( expected != counted ) {
                     throw std::logic_error( "wrong size" );  // NOLINT
                  }
               }

               std::size_t expected;
               std::size_t counted = 0;
            };

            state_t state = EXPECT_TOP_LEVEL_VALUE;
            std::vector< state_t > stack;
            std::vector< sizes_t > sizes;

            void atom( const std::string& function )
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     state = EXPECT_NOTHING;
                     return;
                  case EXPECT_ARRAY_VALUE_OR_END:
                     state = EXPECT_ARRAY_ELEMENT;
                     return;
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     state = EXPECT_SIZED_ARRAY_ELEMENT;
                     return;
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but " + function + " was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but " + function + " was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                     state = EXPECT_OBJECT_MEMBER;
                     return;
                  case EXPECT_SIZED_OBJECT_VALUE:
                     state = EXPECT_SIZED_OBJECT_MEMBER;
                     return;
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but " + function + " was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but " + function + " was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

         public:
            bool is_complete() const noexcept
            {
               return state == EXPECT_NOTHING;
            }

            void null()
            {
               atom( "null()" );
            }

            void boolean( const bool /*unused*/ )
            {
               atom( "boolen()" );
            }

            void number( const std::int64_t /*unused*/ )
            {
               atom( "number(std::int64_t)" );
            }

            void number( const std::uint64_t /*unused*/ )
            {
               atom( "number(std::uint64_t)" );
            }

            void number( const double /*unused*/ )
            {
               atom( "number(double)" );
            }

            void string( const tao::string_view /*unused*/ )
            {
               atom( "string(...)" );
            }

            void binary( const tao::binary_view /*unused*/ )
            {
               atom( "binary(...)" );
            }

            void local_date( const json::local_date /*unused*/ )
            {
               atom( "local_date(...)" );
            }

            void local_time( const json::local_time /*unused*/ )
            {
               atom( "local_time(...)" );
            }

            void local_date_time( const json::local_date_time /*unused*/ )
            {
               atom( "local_date_time(...)" );
            }

            void offset_date_time( const json::offset_date_time /*unused*/ )
            {
               atom( "offset_date_time(...)" );
            }

            void begin_array()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     stack.push_back( EXPECT_NOTHING );
                     state = EXPECT_ARRAY_VALUE_OR_END;
                     return;
                  case EXPECT_ARRAY_VALUE_OR_END:
                     stack.push_back( EXPECT_ARRAY_ELEMENT );
                     return;
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     stack.push_back( EXPECT_SIZED_ARRAY_ELEMENT );
                     return;
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but begin_array(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but begin_array(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                     stack.push_back( EXPECT_OBJECT_MEMBER );
                     state = EXPECT_ARRAY_VALUE_OR_END;
                     return;
                  case EXPECT_SIZED_OBJECT_VALUE:
                     stack.push_back( EXPECT_SIZED_OBJECT_MEMBER );
                     state = EXPECT_ARRAY_VALUE_OR_END;
                     return;
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but begin_array(...) was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but begin_array(...) was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void begin_array( const std::size_t expected )
            {
               begin_array();
               sizes.emplace_back( expected );
               state = EXPECT_SIZED_ARRAY_VALUE_OR_END;
            }

            void element()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     throw std::logic_error( "expected any value, but element() was called" );  // NOLINT
                  case EXPECT_ARRAY_VALUE_OR_END:
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected any value or end_array(...), but element() was called" );  // NOLINT
                  case EXPECT_ARRAY_ELEMENT:
                     state = EXPECT_ARRAY_VALUE_OR_END;
                     return;
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     state = EXPECT_SIZED_ARRAY_VALUE_OR_END;
                     ++sizes.back().counted;
                     return;
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but element() was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                  case EXPECT_SIZED_OBJECT_VALUE:
                     throw std::logic_error( "expected any value, but element() was called" );  // NOLINT
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but element() was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but element() was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void end_array()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     throw std::logic_error( "expected any value, but end_array(....) was called" );  // NOLINT
                  case EXPECT_ARRAY_VALUE_OR_END:
                     state = stack.back();
                     stack.pop_back();
                     return;
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected end_array(std::size_t), but end_array() was called" );  // NOLINT
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but end_array(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but end_array(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                  case EXPECT_SIZED_OBJECT_VALUE:
                     throw std::logic_error( "expected any value, but end_array(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but end_array(...) was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but end_array(...) was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void end_array( const std::size_t expected )
            {
               switch( state ) {
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     state = EXPECT_ARRAY_VALUE_OR_END;
                     break;
                  case EXPECT_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected any value or end_array(), but end_array(std::size_t) was called" );  // NOLINT
                  default:
                     break;
               }
               assert( sizes.size() );
               sizes.back().check( expected );
               sizes.pop_back();
               end_array();
            }

            void begin_object()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     stack.push_back( EXPECT_NOTHING );
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_ARRAY_VALUE_OR_END:
                     stack.push_back( EXPECT_ARRAY_ELEMENT );
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     stack.push_back( EXPECT_SIZED_ARRAY_ELEMENT );
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but begin_object(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but begin_object(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                     stack.push_back( EXPECT_OBJECT_MEMBER );
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_SIZED_OBJECT_VALUE:
                     stack.push_back( EXPECT_SIZED_OBJECT_MEMBER );
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but begin_object(...) was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but begin_object(...) was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void begin_object( const std::size_t expected )
            {
               begin_object();
               sizes.emplace_back( expected );
               state = EXPECT_SIZED_OBJECT_KEY_OR_END;
            }

            void key( const tao::string_view /*unused*/ )
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     throw std::logic_error( "expected any value, but key() was called" );  // NOLINT
                  case EXPECT_ARRAY_VALUE_OR_END:
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected any value or end_array(...), but key() was called" );  // NOLINT
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but key() was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                     state = EXPECT_OBJECT_VALUE;
                     return;
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     state = EXPECT_SIZED_OBJECT_VALUE;
                     return;
                  case EXPECT_OBJECT_VALUE:
                  case EXPECT_SIZED_OBJECT_VALUE:
                     throw std::logic_error( "expected any value, but key() was called" );  // NOLINT
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but key() was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but key() was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void member()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     throw std::logic_error( "expected any value, but member() was called" );  // NOLINT
                  case EXPECT_ARRAY_VALUE_OR_END:
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected any value or end_array(...), but member() was called" );  // NOLINT
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but member() was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected key() or end_object(...), but member() was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                  case EXPECT_SIZED_OBJECT_VALUE:
                     throw std::logic_error( "expected any value, but member() was called" );  // NOLINT
                  case EXPECT_OBJECT_MEMBER:
                     state = EXPECT_OBJECT_KEY_OR_END;
                     return;
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     state = EXPECT_SIZED_OBJECT_KEY_OR_END;
                     ++sizes.back().counted;
                     return;
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but member() was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void end_object()
            {
               switch( state ) {
                  case EXPECT_TOP_LEVEL_VALUE:
                     throw std::logic_error( "expected any value, but end_object(...) was called" );  // NOLINT
                  case EXPECT_ARRAY_VALUE_OR_END:
                  case EXPECT_SIZED_ARRAY_VALUE_OR_END:
                     throw std::logic_error( "expected any value or end_array(...), but end_object(...) was called" );  // NOLINT
                  case EXPECT_ARRAY_ELEMENT:
                  case EXPECT_SIZED_ARRAY_ELEMENT:
                     throw std::logic_error( "expected element(), but end_object(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_KEY_OR_END:
                     state = stack.back();
                     stack.pop_back();
                     return;
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected any value or end_object(std::size_t), but end_object() was called" );  // NOLINT
                  case EXPECT_OBJECT_VALUE:
                  case EXPECT_SIZED_OBJECT_VALUE:
                     throw std::logic_error( "expected any value, but end_object(...) was called" );  // NOLINT
                  case EXPECT_OBJECT_MEMBER:
                  case EXPECT_SIZED_OBJECT_MEMBER:
                     throw std::logic_error( "expected member(), but end_object(...) was called" );  // NOLINT
                  case EXPECT_NOTHING:
                     throw std::logic_error( "expected nothing, but end_object(...) was called" );  // NOLINT
               }
               throw std::logic_error( "invalid state" );  // NOLINT
            }

            void end_object( const std::size_t expected )
            {
               switch( state ) {
                  case EXPECT_SIZED_OBJECT_KEY_OR_END:
                     state = EXPECT_OBJECT_KEY_OR_END;
                     break;
                  case EXPECT_OBJECT_KEY_OR_END:
                     throw std::logic_error( "expected any value or end_object(), but end_object(std::size_t) was called" );  // NOLINT
                  default:
                     break;
               }
               assert( sizes.size() );
               sizes.back().check( expected );
               sizes.pop_back();
               end_object();
            }
         };

      }  // namespace events

   }  // namespace json

}  // namespace tao

#endif
