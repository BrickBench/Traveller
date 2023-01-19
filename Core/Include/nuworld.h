#pragma once

#include "numath.h"
#include "nuscene.h"
#include "nuutil.h"
#include "pch.h"
#include <cstdint>
#include <stdint.h>
#include <string>
#include <vector>

const double ANGLE_TO_SHORT = (65535.0/360.0);

struct LEVELDATA_s {
  char path[0x40];
  char name[0x40];
  char e[0xb0];
};

union GameObject_s {
  char total[0x10D8];
  struct {
    char e[0x58];
    uint16_t angle;
    uint16_t angle2;
    nuvec_s pos;
    nuvec_s speed;
    char e6[0x140];
    nuvec_s vec1;
    nuvec_s vec2;
    nuvec_s lastSafeAIPos;
    float stuckTimer; // 0x1d8
    char e2[0xA4]; 
    char playerId; //0x280
    char e3[3];
    char e4[0xB68];
    float powerupTimer;
    char e5[0x274];
    int characterTypeID;
    char e7[0x17];
    char health;
  };
};

union apicharsys_s {
  char total[0x4c];
  struct {
    char e[0x48];
    char* name;
  };
};

struct APIOBJECTSYS_s {
  int count;
  GameObject_s* list;
};

static_assert(sizeof(GameObject_s) == 0x10D8, "GameObject_s is wrong size");

struct WORLDINFO_s {
  char name[128];
  char directory[128];
  char e1[16];
  int mapDataTXTLength;
  bool hasLoaded;
  char e2[8];
  int currentWorldID;
  int alternateLevelInfoID;
  char e3[4];
  LEVELDATA_s *levelData;
  LEVELDATA_s *alternateLevelData;
  char e4[12];
  nugscn_s *sceneData;
};

TRAVELLER_REGISTER_RAW_GLOBAL(0x02913104, void *, _apicharsys);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0093d810, GameObject_s *, _Player);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00802c54, WORLDINFO_s *, _WORLD);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00951ba4, int, _LDataListSize_);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00951b98, LEVELDATA_s*, _LDataList);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00951ba0, LEVELDATA_s*, _NextLevel);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0093d870, bool, _ChangeLevel);

TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x43d890, __cdecl, GameObject_s* , AddDynamicCreature, 
                                uint32_t, nuvec_s*, int, const char*, int*, int*, int, short*, float*, int, int);

TTSLLib inline WORLDINFO_s *getCurrentWorld() { return *_WORLD; }

TTSLLib inline APIOBJECTSYS_s *getCharacterSys(WORLDINFO_s* world) {
  return *reinterpret_cast<APIOBJECTSYS_s**>(reinterpret_cast<int>(world) + 0x298C);
}

TTSLLib inline std::vector<GameObject_s*> getCharactersFromSys(APIOBJECTSYS_s* sys) {
  std::vector<GameObject_s*> objects;

  for (int i = 0; i < sys->count; i++) {
    if (sys->list[i].pos != nuvec_s {0, 0, 0}) {
      objects.push_back(&sys->list[i]);
    }
  }

  return objects;
}

TTSLLib inline std::vector<LEVELDATA_s*> getLoadedAreas() {
  std::vector<LEVELDATA_s*> levels;

  for (int i = 0; i < *_LDataListSize_; i++) {
    auto currentLData = &(*_LDataList)[i];
    levels.push_back(currentLData);
  }

  return levels;
}
