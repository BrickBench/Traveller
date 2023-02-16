#include "CoreMod.h"

#include <bits/chrono.h>
#include <cctype>
#include <chrono>
#include <d3d9.h>
#include <dinput.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <thread>

#include "Configuration.h"
#include "GuiManager.h"
#include "MemWriteUtils.h"
#include "UIUtils.h"
#include "InjectionManager.h"
#include "LuaRegistry.h"
#include "MinHook.h"
#include "Traveller.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "nudebug.h"
#include "numath.h"
#include "nurender.h"
#include "nuutil.h"
#include "nuworld.h"
#include "pch.h"

TRAVELLER_REGISTER_RAW_FUNCTION(0x6d57d0, DIMOUSESTATE2 *, ReadMouse, void);
TRAVELLER_REGISTER_RAW_FUNCTION(0x6e57d0, int, _d3dShowCursor, int);
TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x4f9940, __cdecl, uint32_t,
                                                  _NuFileInitEx, uint32_t,
                                                  uint32_t, uint32_t);
TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x6dfd40, __stdcall, int,
                                                  PresentWrapper);

TRAVELLER_REGISTER_RAW_GLOBAL(0x2976740, BOOL, d3dCore_presentParams_windowed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x02976c44, BOOL, d3dCore_isWindowed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0082647c, int, PCSettings_width);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826480, int, PCSettings_height);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826484, int, PCSettings_screenXPos);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00826488, int, PCSettings_screenYPos);

static _NuFileInitExSignature oldNuFileInitEx = nullptr;
static BOOL(__stdcall *oldShowWindow)(HWND, int);
static PresentWrapperSignature oldPresentWrapper = nullptr;

static int windowWidth;
static int windowHeight;

static int windowXPos;
static int windowYPos;

__stdcall int patchModes() {
  void *d3dCore = reinterpret_cast<void *>(0x29765e8);
  int oldModeCount =
      *reinterpret_cast<int *>(reinterpret_cast<int>(d3dCore) + 0x618);
  D3DDISPLAYMODE *oldModes = *reinterpret_cast<D3DDISPLAYMODE **>(
      reinterpret_cast<int>(d3dCore) + 0x61c);

  auto lastMode = oldModes[oldModeCount - 1];

  auto targetColorFormat = lastMode.Format;
  auto newRates = {static_cast<int>(lastMode.RefreshRate)};

  int newIdx = 0;
  for (auto rate : newRates) {
    oldModes[newIdx].Width = windowWidth;
    oldModes[newIdx].Height = windowHeight;
    oldModes[newIdx].RefreshRate = rate;
    oldModes[newIdx].Format = targetColorFormat;
    newIdx++;
  }

  *reinterpret_cast<int *>(reinterpret_cast<int>(d3dCore) + 0x660) =
      newRates.size() - 1; // selected mode
  *reinterpret_cast<int *>(reinterpret_cast<int>(d3dCore) + 0x618) =
      newRates.size();

  (*oldPresentWrapper)();
  return 0;
}

uint32_t _cdecl _NuFileInitEx_UseAsHook(uint32_t arg1, uint32_t arg2,
                                        uint32_t arg3) {
  *d3dCore_presentParams_windowed = 1;
  *d3dCore_isWindowed = 1;
  *PCSettings_width = windowWidth;
  *PCSettings_height = windowHeight;
  *PCSettings_screenXPos = windowXPos;
  *PCSettings_screenYPos = windowYPos;
  return (*oldNuFileInitEx)(arg1, arg2, arg3);
}

BOOL _stdcall ShowWindow_ForceWindowed(HWND hwnd, int nCmdShow) {
  return (*oldShowWindow)(hwnd, 1);
}

static DIMOUSESTATE2 emptyMouseState{0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
DIMOUSESTATE2 *_fastcall stubReadMouse(void *thisValue) {
  return &emptyMouseState;
}

void CoreMod::applyWindowChangeHooks() {
  if (this->getConfiguration()->getEntry("display", "disableMouseSteal") == "true") {
    MH_CreateHook((LPVOID)ReadMouse, (LPVOID)&stubReadMouse, nullptr);
    MH_EnableHook((LPVOID)ReadMouse);
  }

  if (this->getConfiguration()->getEntry("display", "windowed") == "true") {
    MH_CreateHook((LPVOID)_NuFileInitEx, (LPVOID)&_NuFileInitEx_UseAsHook,
                  reinterpret_cast<LPVOID *>(&oldNuFileInitEx));
    MH_CreateHookApi(L"user32", "ShowWindow", (LPVOID)ShowWindow_ForceWindowed,
                     reinterpret_cast<LPVOID *>(&oldShowWindow));
    MH_CreateHook((LPVOID)PresentWrapper, (LPVOID)&patchModes,
                  reinterpret_cast<LPVOID *>(&oldPresentWrapper));

    MH_EnableHook((LPVOID)PresentWrapper);
    MH_EnableHook((LPVOID)_NuFileInitEx);
    MH_EnableHook((LPVOID)&ShowWindow);
  }
}


Configuration::ConfigurationData CoreMod::getDefaultConfiguration() {
  return {
    {"display",
        {{"windowed", "true"},
         {"disableMouseSteal", "true"},
         {"windowWidth", "1920"},
         {"windowHeight", "1080"},
         {"windowX", "0"},
         {"windowY", "0"}}},
       {"render",
        {{"useCustomShader", "true"}, {"customShaderFile", "shader.hlsl"}}}};
}

void CoreMod::earlyInit() {
  windowWidth = std::stoi(this->getConfiguration()->getEntry("display", "windowWidth"));
  windowHeight =
      std::stoi(this->getConfiguration()->getEntry("display", "windowHeight"));

  windowXPos = std::stoi(this->getConfiguration()->getEntry("display", "windowX"));
  windowYPos = std::stoi(this->getConfiguration()->getEntry("display", "windowY"));

  applyWindowChangeHooks();

  if (this->getConfiguration()->getEntry("render", "useCustomShader") == "true") {
    auto shaderFile = this->getConfiguration()->getEntry("render", "customShaderFile");

    Traveller::log("Reading " + shaderFile);

    std::streampos size;
    char *memblock;

    std::ifstream file(shaderFile,
                       std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
      size = file.tellg();
      memblock = (char *)malloc(size);
      file.seekg(0, std::ios::beg);
      file.read(memblock, size);
      file.close();
      MemWriteUtils::writeSafeUncheckedPtr(0x00746158, (uint32_t)size);
      MemWriteUtils::writeSafeUncheckedPtr(0x0074615D, (uint32_t)memblock);
      Traveller::log("Successfully patched in " + shaderFile);
    } else {
      Traveller::log(
          "Could not find shader file! Skipping shader patch.");
    }
  }

  Traveller::log("Initialized CoreMod");
}


void CoreMod::lateInit() {
}

static bool drawGrid = false;
void CoreMod::lateRender() {
  auto world = getCurrentWorld();
  if (drawGrid && world && *_Player) {
    (*GameAntinode_Debug_DrawGrid)(world);
  }
}

static bool drawDebugWindow = false;
static std::chrono::system_clock::time_point lastTime = std::chrono::system_clock::now();

std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
                   return std::tolower(c);
                 });
  return s;
}

void CoreMod::drawUI() {
  auto world = getCurrentWorld();

  if (world != nullptr && toLower(std::string(world->name)).ends_with("titles")) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(120, 20), ImGuiCond_Always);
    ImGui::Begin("##splash", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("Traveller %s", VERSION_STRING.c_str());
    ImGui::End();
  }

  if (drawDebugWindow) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always);
    ImGui::Begin("Debug");

    auto currentTime = std::chrono::system_clock::now();
    auto difference = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime);
    auto nanosTime = difference.count();
    auto frameRate = (1e9 / static_cast<double>(nanosTime));

    lastTime = currentTime;

    ImGui::Text("Frame time: %.4f ms, Frame rate: %f", nanosTime/1e6, frameRate);

    ImGui::Separator();
    if (ImGui::TreeNode("Characters")) {
      if (world != nullptr && getCharacterSys(world) != nullptr) {
        auto characters = getCharactersFromSys(getCharacterSys(world));
        ImGui::Text("%d characters", characters.size());
        for (int i = 0; i < characters.size(); i++) {
          auto character = characters[i];

          if (ImGui::TreeNode(std::to_string(i).c_str(), "Character %d, player %d", i + 1, character->playerId)) {
            ImGui::Text("Position: (%f %f %f), rotation: %0.2f",
                 character->pos.x, character->pos.y, character->pos.z, character->angle / ANGLE_TO_SHORT);
            ImGui::Text("Velocity: (%f %f %f)", character->speed.x, character->speed.y, character->speed.z);
            ImGui::Text("Vec: (%f %f %f)", character->vec1.x, character->vec1.y, character->vec1.z);
            ImGui::Text("Vec2: (%f %f %f)", character->vec2.x, character->vec2.y, character->vec2.z);
            ImGui::Text("Health: %d", character->health);
            ImGui::Text("Powerup timer: %f", character->powerupTimer);
            ImGui::Text("Last safe AI position: (%f %f %f)", character->lastSafeAIPos.x, character->lastSafeAIPos.y, character->lastSafeAIPos.z);
            ImGui::Text("Stuck timer: %f", character->stuckTimer);
            ImGui::Text("Character type ID: %d", character->characterTypeID);
            ImGui::TreePop();
          }
        }
      }
      
      ImGui::TreePop();
    }
    
    ImGui::End();
  }
}

void CoreMod::onKeyboardInput(int message, int keyCode) {
  if (message == WM_KEYDOWN) {
    if (keyCode == VK_F3) {
      drawDebugWindow = !drawDebugWindow;
    }

    if (keyCode == VK_F9 && false) {
      nuvec_s pos = nuvec_s {0.015f, 0.745f, 1.0f};
      ADDGAMEMSG msg;
      msg.message = "Wow!";
      msg.pos = &pos;
      msg.pos2 = msg.pos;
      msg.f1 = 0.7f;
      msg.f2 = 2.1f;
      msg.unk2 = 0x4028;
      msg.flag = 0x40000000;
      msg.specialObj = nullptr;

      (*AddGameMsg)(&msg);

    }
  }
}

