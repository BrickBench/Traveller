// DllMain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "InjectionManager.h"


#include <cstdio>
#include <imgui.h>
#include <iostream>
#include <tchar.h>

#include "ScriptingLibrary.h"

#ifdef MSVC
#define ASM_BLOCK(a) _asm {#a}
#else
#define ASM_BLOCK(a) asm volatile("#a")
#endif
HMODULE dinput8;
FARPROC dinput8_functions[5];

extern "C" __declspec(naked) void __stdcall jumpDirectInput8Create() {
    ASM_BLOCK(jmp dinput8_functions[0];);
}

extern "C" __declspec(naked) void __stdcall jumpDllCanUnloadNow() {
    ASM_BLOCK(jmp dinput8_functions[1];);
}

extern "C" __declspec(naked) void __stdcall jumpDllGetClassObject() {
    ASM_BLOCK(jmp dinput8_functions[2];);
}

extern "C" __declspec(naked) void __stdcall jumpDllRegisterServer() {
    ASM_BLOCK(jmp dinput8_functions[3];);
}

extern "C" __declspec(naked) void __stdcall jumpDllUnregisterServer() {
    ASM_BLOCK(jmp dinput8_functions[4];);
}

void initializeDllProxy() {
    dinput8 = LoadLibraryA("C:\\Windows\\System32\\dinput8.dll"); 

    // assign addresses of functions
    dinput8_functions[0] = GetProcAddress(dinput8, "DirectInput8Create");
    dinput8_functions[1] = GetProcAddress(dinput8, "DllCanUnloadNow");
    dinput8_functions[2] = GetProcAddress(dinput8, "DllGetClassObject");
    dinput8_functions[3] = GetProcAddress(dinput8, "DllRegisterServer");
    dinput8_functions[4] = GetProcAddress(dinput8, "DllUnregisterServer");
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        initializeDllProxy();
        InjectionManager::injectFunction<&ScriptingLibrary::init, 0x00401230, 0x00428b90>();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

