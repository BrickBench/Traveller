#include "pch.h"
#include "MemWriteUtils.h"

#include <cstdint>

#include "LuaRegistry.h"

namespace MemWriteUtils
{
	int setMemoryPerms(uintptr_t address, int size, int perms)
	{
		int last = 0;
		VirtualProtect(reinterpret_cast<void*>(address), static_cast<SIZE_T>(size), static_cast<DWORD>(perms), reinterpret_cast<PDWORD>(&last));
		return last;
	}
}

TRAVELLER_API_REGISTRY()
{
	lua.set_function("safeReadInt32", &MemWriteUtils::readSafeUncheckedPtr<int32_t>);
	lua.set_function("safeWriteInt32", &MemWriteUtils::writeSafeUncheckedPtr<int32_t>);
	lua.set_function("safeReadInt16", &MemWriteUtils::readSafeUncheckedPtr<int16_t>);
	lua.set_function("safeWriteInt16", &MemWriteUtils::writeSafeUncheckedPtr<int16_t>);
	lua.set_function("safeReadInt8", &MemWriteUtils::readSafeUncheckedPtr<int8_t>);
	lua.set_function("safeWriteInt8", &MemWriteUtils::writeSafeUncheckedPtr<int8_t>);
}
