#include "ScriptingLibrary.h"
#include "pch.h"

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

namespace ScriptingLibrary {
std::map<std::string, std::shared_ptr<BaseMod>> loadedMods;
std::shared_ptr<CoreMod> coreMod;

auto currentTime = std::chrono::high_resolution_clock::now();
bool beginRender = false;

void openConsole() {
  AllocConsole();

  FILE *file = nullptr;
  freopen_s(&file, "CONIN$", "r", stdin);
  freopen_s(&file, "CONOUT$", "w", stdout);
  freopen_s(&file, "CONOUT$", "w", stderr);
  std::cout.clear();
  std::clog.clear();
  std::cerr.clear();
  std::cin.clear();

  HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
  SetStdHandle(STD_ERROR_HANDLE, hConOut);
  SetStdHandle(STD_INPUT_HANDLE, hConIn);

  std::wcout.clear();
  std::wclog.clear();
  std::wcerr.clear();
  std::wcin.clear();
}

std::map<std::string, std::shared_ptr<BaseMod>> loadDllFiles() {
  std::map<std::string, std::shared_ptr<BaseMod>> mods;
  ScriptingLibrary::log("Loading mods");
  std::filesystem::create_directory("plugins");
  for (const auto &dirEntry :
       std::filesystem::recursive_directory_iterator("plugins")) {
    auto path = dirEntry.path().string();
    if (!path.ends_with("dll"))
      continue;

    HINSTANCE temp = LoadLibraryA(path.c_str());

    if (!temp) {
      ScriptingLibrary::log("Couldn't load library " + path);
      continue;
    }

    ScriptingLibrary::log("Loading library " + path);

    typedef BaseMod *(__cdecl * ModGetter)();

    auto address = GetProcAddress(temp, "getModInstance");

    if (address != nullptr) {
      auto objFunc = reinterpret_cast<ModGetter>(address);
      auto mod = objFunc();

      ScriptingLibrary::log("Loaded mod " + mod->getName());

      mods[mod->getName()] = std::shared_ptr<BaseMod>(mod);
    }
  }
  ScriptingLibrary::log("Loaded " + std::to_string(mods.size()) + " mods.");

  return mods;
}

void earlyUpdate() {
  auto newTime = std::chrono::high_resolution_clock::now();
  auto delta =
      std::chrono::duration<double, std::milli>(newTime - currentTime).count();

  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->earlyUpdate(delta);
  }
  currentTime = newTime;
}

void lateUpdate() { InputHandler::updateInputs(); }

void earlyRender() {
  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->earlyRender();
  }
}

void lateRender() {
  if (!beginRender)
    return;

  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->lateRender();
  }
  auto width = std::stoi(coreMod->coreConfig->getEntry("display", "windowWidth"));
  auto height = std::stoi(coreMod->coreConfig->getEntry("display", "windowHeight"));

  ScriptingLibrary::currentModule = "GUIManager";
  Gui::startRender(width, height);

  ScriptingLibrary::currentModule = "GUIManager";
  Gui::endRender();
}

void preWindowInit() {
  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->preWindowInit();
  }
}

void lateInit() {
  ScriptingLibrary::currentModule = "GUIManager";
  Gui::initializeImGui();
  beginRender = true;

  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->lateInit();
  }
}

void earlyInit() {
  // openConsole();
  MH_Initialize();

  coreMod = std::make_shared<CoreMod>();
  loadedMods = loadDllFiles();
  loadedMods["CoreMod"] = coreMod;

  log("Loaded TTScriptingLibrary");

  InjectionManager::initialize();

  InjectionManager::injectFunction<&preWindowInit, 0x6e3948, 0x6df300>();
  InjectionManager::injectFunction<&lateInit, 0x004931d4, 0x00549430>();
  InjectionManager::injectFunction<&lateUpdate, 0x00493533, 0x00548f00>();
  InjectionManager::injectFunction<&lateRender, 0x0060b569, 0x006e4a10>();

  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->earlyInit();
  }
}

void acceptInput(int message, int keyCode) {}

void log(const std::string &str) {
  std::cout << currentModule << ": " << str << std::endl;
  Gui::writeToConsole(currentModule + ": " + str + "\n");
}

void init() {
  currentModule = "InjectionEngine";
  earlyInit();
}

void onKeyboardInput(int message, int keyCode) {
  for (auto &[name, mod] : loadedMods) {
    ScriptingLibrary::currentModule = name;
    mod->onKeyboardInput(message, keyCode);
  }
}

std::shared_ptr<BaseMod> getModByName(const std::string &name) {
  return loadedMods[name];
}
} // namespace ScriptingLibrary

TRAVELLER_API_REGISTRY() { lua.set_function("log", ScriptingLibrary::log); }
