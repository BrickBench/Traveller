#pragma once
#include "nuscene.h"
#include "pch.h"

struct LEVELDATA_s
{
	int e;
};

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

TTSLLib inline WORLDINFO_s* getCurrentWorld()
{
	return *reinterpret_cast<WORLDINFO_s**>(0x00802c54);
}