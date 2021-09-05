#include "pch.h"
#include "ScriptingLibrary.h"

#define SOL_ALL_SAFETIES_ON 1

#include <filesystem>
#include <tchar.h>
#include <thread>

#include "CoreMod.h"
#include "GuiManager.h"
#include "InjectionManager.h"
#include "InputHandler.h"
#include "LuaRegistry.h"
#include "sol/sol.hpp"

namespace ScriptingLibrary
{
    CoreMod coreMod;
    auto currentTime = std::chrono::high_resolution_clock::now();

    void openConsole() {
        AllocConsole();

        FILE* file = nullptr;
        freopen_s(&file, "CONIN$", "r", stdin);
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
        std::cin.clear();

        HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
        SetStdHandle(STD_ERROR_HANDLE, hConOut);
        SetStdHandle(STD_INPUT_HANDLE, hConIn);

        std::wcout.clear();
        std::wclog.clear();
        std::wcerr.clear();
        std::wcin.clear();
    }

    void earlyUpdate()
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration<double, std::milli>(newTime - currentTime).count();

        coreMod.earlyUpdate(delta);

        currentTime = newTime;
    }

    void lateUpdate()
    {
        InputHandler::updateInputs();
    }

    void earlyRender()
    {
        coreMod.earlyRender();
    }

    void lateRender()
    {
        coreMod.lateRender();
    }

    void preWindowInit()
    {
    }


    void lateInit()
    {
        coreMod.lateInit();
    }

    void earlyInit()
    {
        openConsole();

        log("Loaded TTScriptingLibrary");

        InjectionManager::initialize();

        InjectionManager::injectFunction<&preWindowInit, 0x006dbf4c, reinterpret_cast<void*>(0x006d3150)>();
        InjectionManager::injectFunction<&lateInit, 0x004931d4, reinterpret_cast<void*>(0x00549430)>();
        InjectionManager::injectFunction<&lateUpdate, 0x00493533, reinterpret_cast<void*>(0x00548f00)>();
        InjectionManager::injectFunction<&lateRender, 0x0060b569 , reinterpret_cast<void*>(0x006e4a10)>();

        coreMod.earlyInit();
    }


    void log(const std::string& str)
    {
        std::cout << currentModule << ": " << str << std::endl;
    }

    void init()
	{
        currentModule = "InjectionEngine";
        earlyInit();
	}
}


TRAVELLER_API_REGISTRY()
{
    lua.set_function("log", ScriptingLibrary::log);
}