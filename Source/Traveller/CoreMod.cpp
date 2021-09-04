#include "CoreMod.h"

#include <filesystem>
#include <thread>

#include "GuiManager.h"
#include "ScriptingLibrary.h"
#include "InjectionManager.h"
#include "nuworld.h"

bool beginRender = false;

std::map<std::string, std::unique_ptr<sol::table>> loadLuaScripts(sol::state& lua)
{
    ScriptingLibrary::log("Loading scripts");

    std::map <std::string, std::unique_ptr<sol::table>> scripts;

    lua.open_libraries(sol::lib::base);
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("modscripts"))
    {
        if (dirEntry.is_regular_file())
        {
            ScriptingLibrary::log("Loading script " + dirEntry.path().string());
            auto result = lua.safe_script_file(dirEntry.path().string());

            if (!result.valid()) 
            {
                sol::error err = result;
                ScriptingLibrary::log("Failed to execute script " + dirEntry.path().filename().string() + ": " + std::string(err.what()));
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
                    ScriptingLibrary::log("Script " + dirEntry.path().filename().string() + " did not return a table");
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

void CoreMod::registerTypes()
{
	luaState.set_function("log", &ScriptingLibrary::log);

    luaState.set_function("safeReadInt32", &MemWriteUtils::readSafeUncheckedPtr<int32_t>);
    luaState.set_function("safeWriteInt32", &MemWriteUtils::writeSafeUncheckedPtr<int32_t>);
    luaState.set_function("safeReadInt16", &MemWriteUtils::readSafeUncheckedPtr<int16_t>);
    luaState.set_function("safeWriteInt16", &MemWriteUtils::writeSafeUncheckedPtr<int16_t>);
    luaState.set_function("safeReadInt8", &MemWriteUtils::readSafeUncheckedPtr<int8_t>);
    luaState.set_function("safeWriteInt8", &MemWriteUtils::writeSafeUncheckedPtr<int8_t>);

	luaState.set_function("getCurrentWorld", &getCurrentWorld);
    
	luaState.new_usertype<WORLDINFO_s>("WORLDINFO_s",
		"name", &WORLDINFO_s::name,
		"directory", &WORLDINFO_s::directory);
    
	luaState.new_usertype<nuvec_s>("nuvec_s",
		"x", &nuvec_s::x,
		"y", &nuvec_s::y,
		"z", &nuvec_s::z);

    luaState.
 }

void CoreMod::readConsoleStream()
{
    while (true)
    {
        std::string line;
        std::getline(std::cin, line);

        processConsoleScript(line);
    }
	
}

void CoreMod::processConsoleScript(const std::string& script)
{
    auto result = luaState.safe_script(script, sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error err = result;
        ScriptingLibrary::log("Failed to execute script: " + std::string(err.what()));
    }
}


void CoreMod::earlyInit()
{
    ScriptingLibrary::currentModule = this->getName();

    registerTypes();
    this->loadedMods = loadDllFiles();
    this->loadedScripts = loadLuaScripts(luaState);

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

    auto consoleThread = std::thread(&CoreMod::readConsoleStream, this);
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
