#pragma once

#include "pch.h"

namespace MemWriteUtils {
TTSLLib int setMemoryPerms(uintptr_t address, int size, int perms);

template <typename T> TTSLLib T readSafe(T *address) {
  const auto lastPerms = setMemoryPerms(reinterpret_cast<uintptr_t>(address),
                                        sizeof(T), PAGE_READWRITE);
  T value = *address;
  setMemoryPerms(reinterpret_cast<uintptr_t>(address), sizeof(T), lastPerms);

  return value;
}

template <typename T> TTSLLib T readSafeUncheckedPtr(uintptr_t address) {
  return readSafe<T>(reinterpret_cast<T *>(address));
}

template <typename T> TTSLLib void writeSafe(T *address, const T &value) {
  const auto lastPerms = setMemoryPerms(reinterpret_cast<uintptr_t>(address),
                                        sizeof(T), PAGE_READWRITE);
  *address = value;
  setMemoryPerms(reinterpret_cast<uintptr_t>(address), sizeof(T), lastPerms);
}

template <typename T>
TTSLLib void writeSafeUncheckedPtr(uintptr_t address, const T &value) {
  writeSafe<T>(reinterpret_cast<T *>(address), value);
}
}; // namespace MemWriteUtils
