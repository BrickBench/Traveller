#pragma once

#include "pch.h"
#include "sol/state.hpp"

typedef void (*LuaConfigRun)();

TTSLLib extern sol::state lua;
TTSLLib extern std::vector<LuaConfigRun> luaRegistries;

#define LOAD_LUA

#define CONCAT_INTERNAL(A, B) A##B
#define CONCAT(A, B) CONCAT_INTERNAL(A, B)

#define TRAVELLER_API_REGISTRY()                                               \
  static void luaRegistryFunction();                                           \
  namespace {                                                                  \
  struct LuaRegistryImpl {                                                     \
    LuaRegistryImpl() { luaRegistries.push_back(&luaRegistryFunction); }       \
  };                                                                           \
  }                                                                            \
  static const LuaRegistryImpl registryImpl_;                                  \
  static void luaRegistryFunction()
