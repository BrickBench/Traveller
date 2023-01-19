// DllMain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "InjectionManager.h"

#include <cstdio>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <tchar.h>

#include "ScriptingLibrary.h"

HMODULE dinput8;
FARPROC dinput8_create;
FARPROC dinput8_unload;
FARPROC dinput8_get_class;
FARPROC dinput8_register;
FARPROC dinput8_unregister;

#ifdef _MSC_VER
__declspec(naked) void __stdcall jumpDirectInput8Create() {
  _asm {jmp dinput8_create}
}

__declspec(naked) void __stdcall jumpDllCanUnloadNow() {
  _asm {jmp dinput8_unload}
}

__declspec(naked) void __stdcall jumpDllGetClassObject() {
  _asm {jmp dinput8_get_class}
}

__declspec(naked) void __stdcall jumpDllRegisterServer() {
  _asm {jmp dinput8_register}
}

__declspec(naked) void __stdcall jumpDllUnregisterServer() {
  _asm {jmp dinput8_unregister}
}
#else
extern "C" __declspec(naked) void __stdcall jumpDirectInput8Create() {
  asm("jmp *%0" : : "r"(dinput8_create) : "%eax");
}

extern "C" __declspec(naked) void __stdcall jumpDllCanUnloadNow() {
  asm("jmp *%0" : : "r"(dinput8_unload) : "%eax");
}

extern "C" __declspec(naked) void __stdcall jumpDllGetClassObject() {
  asm("jmp *%0" : : "r"(dinput8_get_class) : "%eax");
}

extern "C" __declspec(naked) void __stdcall jumpDllRegisterServer() {
  asm("jmp *%0" : : "r"(dinput8_register) : "%eax");
}

extern "C" __declspec(naked) void __stdcall jumpDllUnregisterServer() {
  asm("jmp *%0" : : "r"(dinput8_unregister) : "%eax");
}
#endif

void initializeDllProxy() {
  dinput8 = LoadLibraryA("C:\\windows\\system32\\dinput8.dll");

  // assign addresses of functions
  dinput8_create = GetProcAddress(dinput8, "DirectInput8Create");
  dinput8_unload = GetProcAddress(dinput8, "DllCanUnloadNow");
  dinput8_get_class = GetProcAddress(dinput8, "DllGetClassObject");
  dinput8_register = GetProcAddress(dinput8, "DllRegisterServer");
  dinput8_unregister = GetProcAddress(dinput8, "DllUnregisterServer");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    initializeDllProxy();
    InjectionManager::injectFunction<&ScriptingLibrary::init, 0x00401230,
                                     0x00428b90>();
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}
