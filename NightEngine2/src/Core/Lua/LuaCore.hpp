/*!
  @file LuaCore.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of LuaCore
*/
#pragma once
#include "Core/Lua/sol.hpp"

namespace Core
{
  namespace LuaCore
  {
    //TODO: Load Script into cache
    //TODO: Script Execution Order
    enum class LuaScope : unsigned
    {
      INIT = 0,
      AWAKE,
      START,
      UPDATE,
      DESTROY
    };

    //! @brief Get Global lua state
    sol::state& GetState();

    void ReloadScript();

    void SafeScript(const std::string& script);

    //! @brief LuaCore Initiliazation
    void Initialize();

    //! @brief LuaCore Terminate
    void Terminate();
  } // LuaCore
} // Core