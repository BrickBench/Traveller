#include "CoreMod.h"

#include <filesystem>
#include <ranges>
#include <regex>
#include <thread>
#include <dinput.h>
#include <d3d9.h>

#include "GuiManager.h"
#include "ScriptingLibrary.h"
#include "InjectionManager.h"
#include "LuaRegistry.h"
#include "nuworld.h"
#include "nudebug.h"
#include "nurender.h"

TRAVELLER_REGISTER_RAW_FUNCTION(0x1ac972, int, _d3dShowCursor, int);
TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x4f9940, __cdecl, uint32_t, _NuFileInitEx, uint32_t, uint32_t, uint32_t);
TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x6e3940, __fastcall, void, CD3DCore_BuildDeviceFromResolution, void*);

TRAVELLER_REGISTER_RAW_GLOBAL(0x2976740, BOOL, d3dCore_presentParams_windowed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x02976c44, BOOL, d3dCore_isWindowed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0082647c, int, PCSettings_width);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826480, int, PCSettings_height);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826484, int, PCSettings_screenXPos);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826488, int, PCSettings_screenYPos);

static CD3DCore_BuildDeviceFromResolutionSignature oldBuildDeviceFromResolution = nullptr;
static _NuFileInitExSignature oldNuFileInitEx = nullptr;
static BOOL(__stdcall *oldShowWindow)(HWND, int);

static bool hasPatchedResolution = false;

void patchModes(void* d3dCore)
{
	int oldModeCount = *reinterpret_cast<int*>(reinterpret_cast<int>(d3dCore) + 0x618);
	D3DDISPLAYMODE* oldModes = *reinterpret_cast<D3DDISPLAYMODE**>(reinterpret_cast<int>(d3dCore) + 0x61c);
	auto lastMode = oldModes[oldModeCount - 1];

	auto targetWidth = lastMode.Width;
	auto targetHeight = lastMode.Height;
	auto targetColorFormat = lastMode.Format;

	auto newRates = { 10, 40, 50, 60, static_cast<int>(lastMode.RefreshRate) };

	int newIdx = 0;
	for (auto rate : newRates)
	{
        oldModes[newIdx].Width = targetWidth;
        oldModes[newIdx].Height = targetHeight;
        oldModes[newIdx].RefreshRate = rate;
        oldModes[newIdx].Format = targetColorFormat;
		newIdx++;
	}

	for (int i = 0; i < newRates.size(); i++)
	{
		auto mode = oldModes[i];
		ScriptingLibrary::log("New mode:" + std::to_string(mode.Width) + " " + std::to_string(mode.Height) + " " + std::to_string(mode.RefreshRate));
	}

	*reinterpret_cast<int*>(reinterpret_cast<int>(d3dCore) + 0x660) = newRates.size()-1; //selected mode
	*reinterpret_cast<int*>(reinterpret_cast<int>(d3dCore) + 0x618) = newRates.size();
}

uint32_t _cdecl _NuFileInitEx_UseAsHook(uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    *d3dCore_presentParams_windowed = 1;
    *d3dCore_isWindowed = 1;
    *PCSettings_width = 1280;
    *PCSettings_height = 720;
    *PCSettings_screenXPos = 100;
    *PCSettings_screenYPos = 100;
    return (*oldNuFileInitEx)(arg1, arg2, arg3);
}

int __fastcall _d3dShowCursor_Disable(int show)
{
    return 0;
}

HCURSOR _stdcall SetCursor_Disable(HCURSOR cursor)
{
    return cursor;
}

BOOL _stdcall SetCursorPos_Disable(int X, int Y)
{
    return true;
}

BOOL _stdcall ShowWindow_ForceWindowed(HWND hwnd, int nCmdShow)
{
    return (*oldShowWindow)(hwnd, 1);
}

void applyWindowChanges()
{
    MH_CreateHook(_d3dShowCursor, &_d3dShowCursor_Disable, nullptr);
    MH_CreateHook(_NuFileInitEx, &_NuFileInitEx_UseAsHook, reinterpret_cast<LPVOID*>(&oldNuFileInitEx));

    MH_CreateHookApi(L"user32", "SetCursor", &SetCursor_Disable, nullptr);
    MH_CreateHookApi(L"user32", "SetCursorPos", &SetCursorPos_Disable, nullptr);
    MH_CreateHookApi(L"user32", "ShowWindow", &ShowWindow_ForceWindowed, reinterpret_cast<LPVOID*>(&oldShowWindow));

    //MH_EnableHook(CD3DCore_BuildDeviceFromResolution);
    MH_EnableHook(_d3dShowCursor);
    MH_EnableHook(&SetCursor);
    MH_EnableHook(&SetCursorPos);
    MH_EnableHook(_NuFileInitEx);
    MH_EnableHook(&ShowWindow);
}

std::map<std::string, std::unique_ptr<sol::table>> loadLuaScripts()
{
    lua.script("mods = {}");

	ScriptingLibrary::log("Loading scripts");
    std::filesystem::create_directory("modscripts");

    std::map <std::string, std::unique_ptr<sol::table>> scripts;

    lua.open_libraries(sol::lib::base);
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("modscripts"))
    {
        if (dirEntry.is_regular_file())
        {
            auto name = dirEntry.path().stem().string();

            auto file = dirEntry.path().string();
            std::ranges::replace(file, '\\', '/');

            auto luaFile = dirEntry.path().parent_path().string() + "/" + dirEntry.path().stem().string();
            std::ranges::replace(luaFile, '\\', '/');

            ScriptingLibrary::log("Loading script " + file);
            //auto result = lua.safe_script_file(file);
            auto result = lua.safe_script("mods." + dirEntry.path().stem().string() + " = require(\"" + luaFile + "\")");
            ScriptingLibrary::log(std::to_string(static_cast<int>(result.get_type())));
            if (!result.valid()) 
            {
                sol::error err = result;
                ScriptingLibrary::log("Failed to execute script " + file + ": " + std::string(err.what()));
            }
        	else
            {
                auto resultObject = lua["mods"][name];

                if (resultObject.get_type() == sol::type::table)
                {
                    auto resultTable = resultObject.get<sol::table>();
                    scripts[dirEntry.path().string()] = std::make_unique<sol::table>(resultTable);
                }
        		else
                {
                    ScriptingLibrary::log("Script " + file + " did not return a table");
                }
            }
        }
    }

    ScriptingLibrary::log("Loaded " + std::to_string(scripts.size()) + " scripts");

    return scripts;
}

void CoreMod::runScript(const std::string& name, const std::string& func)
{
    auto namespac = *this->loadedScripts[name];
	if (namespac[func].get_type() == sol::type::function)
	{
        auto result = namespac[func].get<sol::safe_function>()();
        if (!result.valid())
        {
            sol::error err = result;
            ScriptingLibrary::log("Failed to execute script " + name + ": " + std::string(err.what()));
        }
	}
}

void registerTypes()
{
    for (auto const& func : luaRegistries)
    {
        (*func)();
    }

    lua.set_function("getCurrentWorld", &getCurrentWorld);
    
    lua.new_usertype<WORLDINFO_s>("WORLDINFO_s",
		"name", &WORLDINFO_s::name,
		"directory", &WORLDINFO_s::directory);
    
    lua.new_usertype<nuvec_s>("nuvec_s",
		"x", &nuvec_s::x,
		"y", &nuvec_s::y,
		"z", &nuvec_s::z);
 }

void processConsoleScript(const std::string& script)
{
    auto sanitized = std::regex_replace(script, std::regex("\""), "\\\"");

    sol::protected_function_result result;
    if (CoreMod::useFennelInterpreter)
    {
        result = lua.safe_script("_scriptResult = fennel.eval(\"" + sanitized + "\")", sol::script_pass_on_error);
    }
    else
    {
        result = lua.safe_script("_scriptResult = " + sanitized, sol::script_pass_on_error);
    }

    if (!result.valid()) {
        sol::error err = result;
        ScriptingLibrary::log("Failed to execute script: " + std::string(err.what()));
    }
	else
    {
        lua.script("log(tostring(_scriptResult))");
    }
}

void readConsoleStream()
{
    while (true)
    {
        std::string line;
        std::getline(std::cin, line);

        processConsoleScript(line);
    }
}

void loadLuaEnvironment()
{
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::io,
        sol::lib::table, sol::lib::string, sol::lib::utf8, sol::lib::math, sol::lib::debug);

    lua["fennel"] = lua.script_file("fennel.lua");
    lua.script("table.insert(package.loaders or package.searchers, fennel.searcher)");
}

void CoreMod::earlyInit()
{

#ifdef LOAD_LUA
    loadLuaEnvironment();
    registerTypes();

	this->loadedScripts = loadLuaScripts();
#endif
    applyWindowChanges();

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyInit");
    }

    ScriptingLibrary::log("Initialized CoreMod");
}

void CoreMod::lateInit()
{
    patchModes(reinterpret_cast<void*>(0x029765e8));

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "lateInit");
    }
}

void CoreMod::earlyUpdate(double delta)
{
    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyUpdate");
    }
}

void CoreMod::earlyRender()
{
    for (const auto& name : loadedScripts | std::views::keys)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyRender");
    }
}

static bool drawGrid = false;
void CoreMod::lateRender()
{
    for (const auto& name : loadedScripts | std::views::keys)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "lateRender");
    }

    auto world = getCurrentWorld();
    if (drawGrid && world && *_Player)
    {
        (*GameAntinode_Debug_DrawGrid)(world);
    }
}

void CoreMod::onKeyboardInput(int message, int keyCode)
{
	if (message == WM_KEYDOWN)
	{
        if (keyCode == 'G')
        {
            typedef void(*mystery)();
            (*reinterpret_cast<mystery>(0x688d70))();
            ScriptingLibrary::log(std::to_string(*((int*)0x29620f4)));
        }

		if (keyCode == 'N')
		{
            if (*reinterpret_cast<int*>(0x7f1138) != 0)
            {
                *reinterpret_cast<int*>(0x7f1138) = 0;
            }
			else
            {
                *reinterpret_cast<int*>(0x7f1138) = 0x009253d0;
                *reinterpret_cast<nuvec_s*>(0x7f1118) = { 0,0,0 };
            }
		}
	}
}

void CoreMod::execScript(const std::string& script)
{
    processConsoleScript(script);
}


