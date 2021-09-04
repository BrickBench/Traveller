#pragma once

#include "pch.h"

namespace MemWriteUtils
{
	TTSLLib int setMemoryPerms(int address, int size, int perms);

	template <typename T>
	TTSLLib T readSafe(const T& address)
	{
		auto lastPerms = setMemoryPerms(reinterpret_cast<int>(address), sizeof(T), PAGE_READWRITE);
		T value = *address;
		setMemoryPerms(reinterpret_cast<int>(address), sizeof(T), lastPerms);

		return value;
	}

	template <typename T>
	TTSLLib void writeSafe(T* address, const T& value)
	{
		auto lastPerms = setMemoryPerms(reinterpret_cast<int>(address), sizeof(T), PAGE_READWRITE);
		*address = value;
		setMemoryPerms(reinterpret_cast<int>(address), sizeof(T), lastPerms);
	}
};
