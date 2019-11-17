/*!
  @file LuaCore.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of LuaCore
*/

#include "Core/Lua/LuaCore.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"
#include "Core/Serialization/FileSystem.hpp"

#include "Core/Message/MessageSystem.hpp"
#include "Core/Message/MessageObjectList.hpp"

#include "Input/Input.hpp"

#define START_ENUM_TO_LUA(ENUMTYPE) \
lua.new_enum(#ENUMTYPE

#define END() \
 )

namespace Core
{
  namespace LuaCore
  {
    sol::state& GetState()
    {
      static sol::state lua;
      return lua;
    }

    void SafeScript(const std::string& script)
    {
      auto pfr = GetState().safe_script(script, &sol::script_pass_on_error);
      sol::error err = pfr;
      Debug::Log << err.what() << '\n';
    }

    void ReloadScript()
    {
    
    }

    void Initialize()
    {

      //***********************************************
      //	Test Lua Binding
      //***********************************************
      sol::state& lua = LuaCore::GetState();
      //lua.open_libraries(sol::lib::base);

      //Overload print for debuging
      lua.set_function("print", sol::overload(
        [](const std::string& toPrint) {Debug::Log << toPrint; }
      , [](int toPrint) {Debug::Log << toPrint; }
      , [](unsigned toPrint) {Debug::Log << toPrint; }
      , [](float toPrint) {Debug::Log << toPrint; }));

      lua.set_function("move", sol::overload(
        [](const std::string& name, float x, float y, float z) 
          {
            Debug::Log << Logger::MessageType::LUA << "move: \"" << name 
              << "\" to vec3("
              << x << ", " << y << ", " << z << ")\n";
            TransformMessage msg(name, TransformMessage::TransformType::TRANSLATE
              , glm::vec3(x, y, z));
            MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::HANDLER);
          } 
      ));

      lua.set_function("scale", sol::overload(
        [](const std::string& name, float x, float y, float z)
      {
        Debug::Log << Logger::MessageType::LUA << "scale: \"" << name
          << "\" to vec3("
          << x << ", " << y << ", " << z << ")\n";
        TransformMessage msg(name, TransformMessage::TransformType::SCALE
          , glm::vec3(x, y, z));
        MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::HANDLER);
      }
      ));

      lua.set_function("rotate", sol::overload(
        [](const std::string& name, float x, float y, float z)
      {
        Debug::Log << Logger::MessageType::LUA << "rotate: \"" << name
          << "\" to vec3("
          << x << ", " << y << ", " << z << ")\n";
        TransformMessage msg(name, TransformMessage::TransformType::ROTATE
          , glm::vec3(x, y, z));
        MessageSystem::Get().BroadcastMessage(msg, BroadcastScope::HANDLER);
      }
      ));

      //Register Input KeyCode Enums
      {
#define REGISTER_INPUTKEY(VAR) ,STR_CAT(KEY_,VAR), KeyCode::KEY_##VAR
        using namespace Input;
        START_ENUM_TO_LUA(KeyCode)
#include "Input/RegisteredInputKey.inl"
        END();
#undef REGISTER_INPUTKEY
      }

      //Load Script
      std::string luaScript{ FileSystem::OpenFileAsString("Test.lua", FileSystem::DirectoryType::Script) };
      lua.script(luaScript);

      auto f = lua["Test"];
      f.call();
    }

    void Terminate()
    {
    
    }
  } // LuaCore
} // Core