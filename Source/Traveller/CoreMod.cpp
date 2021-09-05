#include "CoreMod.h"

#include <filesystem>
#include <thread>

#include "ScriptingLibrary.h"
#include "InjectionManager.h"
#include "LuaRegistry.h"
#include "nuworld.h"
#include "nuscene.h"
#include "nuutil.h"

bool beginRender = false;


std::map<std::string, std::unique_ptr<sol::table>> loadLuaScripts()
{
    ScriptingLibrary::log("Loading scripts");
    std::filesystem::create_directory("modscripts");

    std::map <std::string, std::unique_ptr<sol::table>> scripts;

    lua.open_libraries(sol::lib::base);
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("modscripts"))
    {
        if (dirEntry.is_regular_file())
        {
            auto file = dirEntry.path().parent_path().string() + "." + dirEntry.path().stem().string();
            std::replace(file.begin(), file.end(), '\\', '/');

            ScriptingLibrary::log("Loading script " + file);
            auto result = lua.safe_script("require(\"" + file + "\")");

            if (!result.valid()) 
            {
                sol::error err = result;
                ScriptingLibrary::log("Failed to execute script " + file + ": " + std::string(err.what()));
            }
        	else
            {
                if (result.get_type() == sol::type::table)
                {
                    auto resultTable = result.get<sol::table>();
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

std::map <std::string, std::unique_ptr<BaseMod>> loadDllFiles()
{
    std::map <std::string, std::unique_ptr<BaseMod>> mods;
    ScriptingLibrary::log("Loading mods");
    std::filesystem::create_directory("plugins");
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("plugins")) {
        auto path = dirEntry.path().string();
        if (!path.ends_with("dll")) continue;

        HINSTANCE temp = LoadLibraryA(path.c_str());

        if (!temp) {
            ScriptingLibrary::log("Couldn't load library " + path);
            continue;
        }

        ScriptingLibrary::log("Loading library " + path);

        typedef BaseMod* (__cdecl* ModGetter)();

        auto address = GetProcAddress(temp, "getModInstance");

        if (address != nullptr)
        {
            auto objFunc = reinterpret_cast<ModGetter>(address);
            auto mod = objFunc();

            ScriptingLibrary::log("Loaded mod " + mod->getName());

            mods[mod->getName()] = std::unique_ptr<BaseMod>(mod);
        }
    }
    ScriptingLibrary::log("Loaded " + std::to_string(mods.size()) + " mods.");

    return mods;
}

void CoreMod::runScript(const std::string& name, const std::string& func)
{
    auto namespac = *this->loadedScripts[name];
	if (namespac[func].get_type() == sol::type::function)
	{
        auto result = namespac[func].get<sol::function>()();
        if (!result.valid())
        {
            sol::error err = result;
            ScriptingLibrary::log("Failed to execute script " + name + ": " + std::string(err.what()));
        }
	}
}

void registerTypes()
{
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
    auto result = lua.safe_script("_scriptResult = fennel.eval(\"" + script + "\")", sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error err = result;
        ScriptingLibrary::log("Failed to execute script: " + std::string(err.what()));
    }
	else
    {
        lua.script("print(_scriptResult)");
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
        sol::lib::table, sol::lib::string, sol::lib::math);

   lua["fennel"] = lua.script_file("fennel.lua");
   lua.script("table.insert(package.loaders or package.searchers, fennel.searcher)");
}

void CoreMod::earlyInit()
{
    ScriptingLibrary::currentModule = this->getName();

    loadLuaEnvironment();
    registerTypes();
    this->loadedMods = loadDllFiles();
    this->loadedScripts = loadLuaScripts();

    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyInit();
    }

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyInit");
    }
}

void CoreMod::lateInit()
{
    ScriptingLibrary::currentModule = "GUIManager";
   // Gui::initializeImGui();

    auto consoleThread = std::thread(&readConsoleStream);
    consoleThread.detach();

    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->lateInit();
    }

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "lateInit");
    }

    beginRender = true;
}

void CoreMod::earlyUpdate(double delta)
{
    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyUpdate(delta);
    }

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyUpdate");
    }
}

void CoreMod::earlyRender()
{
    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyRender();
    }

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "earlyRender");
    }
}

void CoreMod::lateRender()
{
   // if (!beginRender) return;

    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->lateRender();
    }

    for (auto& [name, script] : loadedScripts)
    {
        ScriptingLibrary::currentModule = name;
        runScript(name, "lateRender");
    }

  //  ScriptingLibrary::currentModule = "GUIManager";
  //  Gui::startRender();

   // drawUI();

    //ScriptingLibrary::currentModule = "GUIManager";
   // Gui::endRender();
}

void CoreMod::drawUI()
{
    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->drawUI();
    }
}
