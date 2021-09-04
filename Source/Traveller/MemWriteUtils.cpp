#include "pch.h"
#include "MemWriteUtils.h"

namespace MemWriteUtils
{
	int setMemoryPerms(uintptr_t address, int size, int perms)
	{
		int last = 0;
		VirtualProtect(reinterpret_cast<void*>(address), static_cast<SIZE_T>(size), static_cast<DWORD>(perms), reinterpret_cast<PDWORD>(&last));
		return last;
	}
}

