#pragma once
#include "numath.h"

struct nugscn_s;

struct GameModel_
{
	int meshCount;
	int* materials;
	int* commands;
};

struct Bounds_
{
	nuvec_s pos;
	int unknown;
	nuvec_s size;
	int unknown2;
};

struct PosBounds_
{
	numtx_s transform;
	Bounds_ bounds;
};

typedef void* GeneratedSpecialObject_;

struct FileSpecialObject_
{
	numtx_s initialTransform;
	PosBounds_* transform;
	nuvec4_s_ unknown;
	GameModel_* model;
	char* name;
	void* visibilityFunction;
	float* lodDistances;
	int boundingBoxIndex;
	PosBounds_* sharedTransform;
	short windShearFactor;
	short windSpeedFactor;
	int unknown2;
};

struct nuhspecial_s
{
	nugscn_s* scene;
	void* object1;
	FileSpecialObject_* object2;
};

struct nugscn_s
{
	int* textureIndices;
	int textureCount;
	void* textureData;
	void* materials;
	int materialCount;
};