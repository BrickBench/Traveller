#include "pch.h"

#include <imgui.h>

#include "Mod.h"
#include "MemWriteUtils.h"
#include "nuworld.h"
#include "UIUtils.h"
#include "Traveller.h"

class CheatMenu : public Mod {
private:
  std::vector<LEVELDATA_s*> areas;
   
  bool drawCheatWindow = false;
  bool drawDebugWindow = false;

  bool skipGameCuts = false;
 
  bool enableLevelLoadHotkey = false;
  LEVELDATA_s* currentLevel = nullptr;
 
  int fRandByValue = 0;
  int fRandByIter = 0;
  nuvec_s playerSaves[6];
  nuvec_s spawnPos;
  int spawnId;
  std::string spawnScript;

  bool resetDoor;
  bool resetLevel;

public:
  std::string getName() override { return "CheatMenu"; }

  Configuration::ConfigurationData getDefaultConfiguration() override {
    return {
      {
        "cheats",
        {
          {"skipIntro", "true"},
        }
      }
    };
  }

  void earlyInit() override {
    if (this->getConfiguration()->getEntry("cheats", "skipIntro") == "true") {
     // MemWriteUtils::writeSafeUncheckedPtr(0x87B53C, 1);
    }
  }

  void lateInit() override {
    //MemWriteUtils::writeSafeUncheckedPtr(0x87B53C, 0);
    areas = getLoadedAreas();
  }

  void earlyUpdate(double delta) override {
  }

  
  void drawUI() override {
    if (drawCheatWindow) {
      ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Always);
      ImGui::Begin("Cheats");

      ImGui::Text("Don Cheatle Menu 2");
      if (ImGui::CollapsingHeader("Level Cheats")) {
        ImGui::Text("Got %d loaded maps", *_LDataListSize_);
       
        if (ImGui::BeginListBox("##Levels")) {
          for (auto level : areas) {
            auto isSelected = (level == currentLevel);
            if (ImGui::Selectable(level->name, isSelected)) {
              currentLevel = level;
            }

            if (isSelected) {
              ImGui::SetItemDefaultFocus();
            }
          }

          ImGui::EndListBox();
        }

        ImGui::Checkbox("Reset door", &resetDoor);
        ImGui::Checkbox("Reset level state", &resetLevel);

        if (ImGui::Button("Go to level") && currentLevel != nullptr) {
          if (resetDoor) {
            *_ResetDoorBit = 0;
          }

          if (resetLevel) {
            *_ResetBit1 = -1;
            *_ResetBit2 = 32;
          }

          *_NextLevel = currentLevel;
          *_ChangeLevel = true;
        }

        ImGui::SameLine();

        ImGui::Checkbox("Enable level load hotkey (|\\)", &enableLevelLoadHotkey);
      }

      if (ImGui::CollapsingHeader("Game Cheats")) {
        if (ImGui::TreeNode("Characters")) {
          for (int i = 0; i < 12 && _Player[i] != nullptr; i++) {
            auto character = _Player[i];
            if (ImGui::TreeNode(std::to_string(i).c_str(), "Player %d", i + 1)) {
              ImGui::SameLine();
              UIUtils::InputNuVec("##Teleport", playerSaves[i]);
              if (ImGui::Button("Teleport")) {
                character->pos = playerSaves[i];
              }
              ImGui::SameLine();
              if (ImGui::Button("Copy position")) {
                playerSaves[i] = character->pos;
              }

              if (ImGui::Button("Force SuperJump")) {
                character->stuckTimer = 4;
              }

              ImGui::InputScalar("Health", ImGuiDataType_U8, &character->health);
              ImGui::InputFloat("Powerup timer", &character->powerupTimer);

              ImGui::TreePop();
            }
          }

          ImGui::TreePop();
        }

        if (ImGui::TreeNode("Spawn Character")) {
          UIUtils::InputNuVec("Spawn position", spawnPos);
          ImGui::InputInt("Character ID", &spawnId);
          ImGui::InputText("Script", &spawnScript);
          
          if (ImGui::Button("Spawn")) {
            (*AddDynamicCreature)(spawnId, &spawnPos, 0, spawnScript.c_str(), nullptr, nullptr, 0, nullptr, nullptr, 0, 0);
          }

          ImGui::TreePop();
        }
 
        if (ImGui::TreeNode("Randomness")) {
          ImGui::Text("QSeed: %s", std::to_string(*_qseed).c_str());
          ImGui::Text("FSeed: %s", std::to_string(*_fseed).c_str());

          ImGui::Text("Set by:");
          ImGui::PushItemWidth(50);
          ImGui::SameLine();
          if (ImGui::InputInt("Value", &fRandByValue)) {
            *_fseed = fRandByValue;
          }

          ImGui::SameLine();
          if (ImGui::InputInt("Iteration", &fRandByIter)) {
            *_fseed = 0;
            for (int i = 0; i < fRandByIter; i++) {
              int mid = (*_fseed ^ 0x75bd924) * 0x41a7 + ((*_fseed ^ 0x75bd924) / 0x1f31d) * -0x7fffffff;
              if (mid < 0) {
                *_fseed = (mid + 0x7fffffff) ^ 0x75bd924;
               } else {
                *_fseed = mid ^ 0x75bd924;
              }
            }
          }
          ImGui::PopItemWidth();

          ImGui::Text("DebrisSeed: %s", std::to_string(*_debrisSeed_).c_str());
          ImGui::Text("PartSeed: %s", std::to_string(*_partSeed_).c_str());
          ImGui::TreePop();
        }
      }

      ImGui::End();
    }
  }

  void onKeyboardInput(int message, int keyCode) override {
    if (message == WM_KEYDOWN) {
      if (enableLevelLoadHotkey && keyCode == VK_OEM_5 && currentLevel != nullptr) {
        *_NextLevel = currentLevel;
        *_ChangeLevel = true;
      }

      if (keyCode == VK_F5) {
        drawCheatWindow = !drawCheatWindow;
      }
    }
  }
};

extern "C" __declspec(dllexport) Mod *getModInstance() { return new CheatMenu(); }
