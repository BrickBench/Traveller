#include "CoreMod.h"

#include <filesystem>
#include <thread>

#include "GuiManager.h"
#include "ScriptingLibrary.h"
#include "InjectionManager.h"
#include "nuworld.h"

bool beginRender = false;

std::map<std::string, std::unique_ptr<sol::load_result>> loadLuaScripts(sol::state& lua)
{
    ScriptingLibrary::log("Loading scripts");

    std::map <std::string, std::unique_ptr<sol::load_result>> scripts;

    lua.open_libraries(sol::lib::base);
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("modscripts"))
    {
        if (dirEntry.is_regular_file())
        {
            ScriptingLibrary::log("Loading script " + dirEntry.path().string());
            scripts[dirEntry.path().string()] = std::make_unique<sol::load_result>(lua.load_file(dirEntry.path().string()));
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

void CoreMod::readConsoleStream()
{
    while (true)
    {
        std::string line;
        std::cin >> line;
        std::cout << std::endl;

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

    this->loadedMods = loadDllFiles();
    this->loadedScripts = loadLuaScripts(luaState);

    

    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyInit();
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

    beginRender = true;
}

void CoreMod::earlyUpdate(double delta)
{
    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyUpdate(delta);
    }
}

void CoreMod::earlyRender()
{
    for (auto& [name, mod] : loadedMods)
    {
        ScriptingLibrary::currentModule = name;
        mod->earlyRender();
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
