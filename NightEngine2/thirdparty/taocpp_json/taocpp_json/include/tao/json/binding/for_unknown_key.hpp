// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_BINDING_FOR_UNKNOWN_KEY_HPP
#define TAO_JSON_BINDING_FOR_UNKNOWN_KEY_HPP

namespace tao
{
   namespace json
   {
      namespace binding
      {
         enum class for_unknown_key : bool
         {
            THROW,
            CONTINUE
         };

      }  // namespace binding

   }  // namespace json

}  // namespace tao

#endif
