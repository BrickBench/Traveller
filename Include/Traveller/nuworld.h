#pragma once

#include "nuutil.h"
#include "nuscene.h"
#include "pch.h"

struct LEVELDATA_s
{
	int e;
};

typedef void* GameObject_s;

struct WORLDINFO_s
{
	char name[128];
	char directory[128];
	char e1[16];
	int mapDataTXTLength;
	bool hasLoaded;
	char e2[8];
	int currentWorldID;
	int alternateLevelInfoID;
	char e3[4];
	LEVELDATA_s* levelData;
	LEVELDATA_s* alternateLevelData;
	char e4[12];
	nugscn_s* sceneData;
};

TRAVELLER_REGISTER_RAW_GLOBAL(0x0093d810, GameObject_s*, _Player);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00802c54, WORLDINFO_s*, _WORLD);

TTSLLib inline WORLDINFO_s* getCurrentWorld()
{
	return *_WORLD;
}