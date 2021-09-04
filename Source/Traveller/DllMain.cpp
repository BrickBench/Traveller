// DllMain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "InjectionManager.h"


#include <cstdio>
#include <imgui.h>
#include <iostream>
#include <tchar.h>

#include "ScriptingLibrary.h"

HMODULE dinput8;
FARPROC dinput8_functions[5];

__declspec(naked) void __stdcall jumpDirectInput8Create() {
    _asm {
        jmp dinput8_functions[0];
    }
}

__declspec(naked) void __stdcall jumpDllCanUnloadNow() {
    _asm {
        jmp dinput8_functions[1];
    }
}

__declspec(naked) void __stdcall jumpDllGetClassObject() {
    _asm {
        jmp dinput8_functions[2];
    }
}

__declspec(naked) void __stdcall jumpDllRegisterServer() {
    _asm {
        jmp dinput8_functions[3];
    }
}

__declspec(naked) void __stdcall jumpDllUnregisterServer() {
    _asm {
        jmp dinput8_functions[4];
    }
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
        InjectionManager::injectFunction<&ScriptingLibrary::init, 0x00401230, reinterpret_cast<void*>(0x00428b90)>();
        initializeDllProxy();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

