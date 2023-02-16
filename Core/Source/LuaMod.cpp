
#include "LuaRegistry.h"
#include "Traveller.h"
#include "LuaMod.h"
#include "nuworld.h"
#include <filesystem>
#include <map>
#include <memory>
#include <regex>
#include <sol/table.hpp>
#include <string>

std::map<std::string, std::unique_ptr<sol::table>> loadLuaScripts() {
  lua.script("mods = {}");

  Traveller::log("Loading scripts");
  std::filesystem::create_directory("modscripts");

  std::map<std::string, std::unique_ptr<sol::table>> scripts;

  lua.open_libraries(sol::lib::base);
  for (const auto &dirEntry :
       std::filesystem::recursive_directory_iterator("modscripts")) {
    if (dirEntry.is_regular_file()) {
      auto name = dirEntry.path().stem().string();

      auto file = dirEntry.path().string();
      std::ranges::replace(file, '\\', '/');

      auto luaFile = dirEntry.path().parent_path().string() + "/" +
                     dirEntry.path().stem().string();
      std::ranges::replace(luaFile, '\\', '/');

      Traveller::log("Loading script " + file);
      auto result = lua.safe_script("mods." + dirEntry.path().stem().string() +
                                    " = require(\"" + luaFile + "\")");
      Traveller::log(
          std::to_string(static_cast<int>(result.get_type())));
      if (!result.valid()) {
        sol::error err = result;
        Traveller::log("Failed to execute script " + file + ": " +
                              std::string(err.what()));
      } else {
        auto resultObject = lua["mods"][name];

        if (resultObject.get_type() == sol::type::table) {
          auto resultTable = resultObject.get<sol::table>();
          scripts[dirEntry.path().string()] =
              std::make_unique<sol::table>(resultTable);
        } else {
          Traveller::log("Script " + file + " did not return a table");
        }
      }
    }
  }

  Traveller::log("Loaded " + std::to_string(scripts.size()) +
                        " scripts");

  return scripts;
}

void LuaMod::runScript(const std::string &name, const std::string &func) {
  auto namespac = *this->loadedScripts[name];
  if (namespac[func].get_type() == sol::type::function) {
    auto result = namespac[func].get<sol::safe_function>()();
    if (!result.valid()) {
      sol::error err = result;
      Traveller::log("Failed to execute script " + name + ": " +
                            std::string(err.what()));
    }
  }
}

void registerTypes() {
  for (auto const &func : luaRegistries) {
    (*func)();
  }

  lua.set_function("getCurrentWorld", &getCurrentWorld);

  lua.new_usertype<WORLDINFO_s>("WORLDINFO_s", "name", &WORLDINFO_s::name,
                                "directory", &WORLDINFO_s::directory);

  lua.new_usertype<nuvec_s>("nuvec_s", "x", &nuvec_s::x, "y", &nuvec_s::y, "z",
                            &nuvec_s::z);
}

Configuration::ConfigurationData LuaMod::getDefaultConfiguration() {
  return  {
    {"scripting",
      {{"enableScripting", "true"}, {"loadFennel", "false"}}}
  };
}

void LuaMod::earlyInit() {
  if (this->getConfiguration()->getEntry("scripting", "enableScripting") == "true") {
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::io,
                       sol::lib::table, sol::lib::string, sol::lib::utf8,
                       sol::lib::math, sol::lib::debug);

    if (this->getConfiguration()->getEntry("scripting", "loadFennel") == "true") {
      lua["fennel"] = lua.script_file("fennel.lua");
      lua.script("table.insert(package.loaders or package.searchers, "
                 "fennel.searcher)");
    }

    registerTypes();

    this->loadedScripts = loadLuaScripts();
  }

  for (auto &[name, script] : loadedScripts) {
    Traveller::currentModule = name;
    runScript(name, "earlyInit");
  }
}


void LuaMod::lateInit() {
  for (auto &[name, script] : loadedScripts) {
    Traveller::currentModule = name;
    runScript(name, "lateInit");
  }
}

void LuaMod::earlyUpdate(double delta) {
  for (auto &[name, script] : loadedScripts) {
    Traveller::currentModule = name;
    runScript(name, "earlyUpdate");
  }
}

void LuaMod::earlyRender() {
  for (auto &[name, script] : loadedScripts) {
    Traveller::currentModule = name;
    runScript(name, "earlyRender");
  }
}

void LuaMod::lateRender() {
  for (auto &[name, script] : loadedScripts) {
    Traveller::currentModule = name;
    runScript(name, "lateRender");
  }
}

void LuaMod::execScript(const std::string &script) {
  auto sanitized = std::regex_replace(script, std::regex("\""), "\\\"");

  sol::protected_function_result result;
  if (LuaMod::useFennelInterpreter) {
    result =
        lua.safe_script("_scriptResult = fennel.eval(\"" + sanitized + "\")",
                        sol::script_pass_on_error);
  } else {
    result = lua.safe_script("_scriptResult = " + sanitized,
                             sol::script_pass_on_error);
  }

  if (!result.valid()) {
    sol::error err = result;
    Traveller::log("Failed to execute script: " +
                          std::string(err.what()));
  } else {
    lua.script("log(tostring(_scriptResult))");
  }
}
