#include "CoreMod.h"

#include <filesystem>
#include <regex>
#include <thread>

#include "GuiManager.h"
#include "ScriptingLibrary.h"
#include "InjectionManager.h"
#include "LuaRegistry.h"
#include "nuworld.h"
#include "nudebug.h"


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
    for (auto func : luaRegistries)
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


    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyInit");
    }

    ScriptingLibrary::log("Initialized CoreMod");
}

void CoreMod::lateInit()
{
#ifdef LOAD_LUA
   // auto consoleThread = std::thread(&readConsoleStream);
   // consoleThread.detach();
#endif

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
    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyRender");
    }
}

static bool drawGrid = false;
void CoreMod::lateRender()
{
    for (auto& [name, script] : loadedScripts)
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
            drawGrid = true;
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


