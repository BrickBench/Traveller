#pragma once

#include "sol/state.hpp"

inline sol::state lua;

#define CONCAT_INTERNAL(A, B) A##B
#define CONCAT(A, B) CONCAT_INTERNAL(A, B)
#define TRAVELLER_API_REGISTRY()\
static void luaRegistryFunction();  \
namespace                           \
{                                   \
    struct LuaRegistryImpl          \
    {                               \
        LuaRegistryImpl()           \
        {                           \
            luaRegistryFunction();  \
        }                           \
    };                              \
}                                   \
static const LuaRegistryImpl registryImpl_;  \
static void luaRegistryFunction()