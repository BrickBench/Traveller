#pragma once

#include "numath.h"
#include "nuscene.h"
#include "nuutil.h"
#include "nuworld.h"

union ADDGAMEMSG {
  char total[80];
  struct {
    const char* message;
    nuvec_s* pos;
    nuvec_s* pos2;
    float f1;
    float f2;
    int unk;
    int unk2;
    int flag;
    char unk3[8];
    nuhspecial_s* specialObj;
  };
};


TRAVELLER_REGISTER_RAW_GLOBAL(0x029134c4, nuvec_s, debugCrossPos);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0080ff3c, float, debugSize);
TRAVELLER_REGISTER_RAW_GLOBAL(0x009739ac, bool, _draw_portals);

TRAVELLER_REGISTER_RAW_FUNCTION(0x00521800, void, FUN_00521800);
TRAVELLER_REGISTER_RAW_FUNCTION(0x0063c810, void, DrawCross);
TRAVELLER_REGISTER_RAW_FUNCTION(0x0059a950, void, GameAntinode_Debug_DrawGrid,
                                WORLDINFO_s *world);
TRAVELLER_REGISTER_RAW_FUNCTION(0x005dc050, void, _NuRndrLine3dDbg, float x1,
                                float y1, float z1, float x2, float y2,
                                float z2);
TRAVELLER_REGISTER_RAW_FUNCTION_CUSTOM_CONVENTION(0x00560b60, __cdecl, void, AddGameMsg, ADDGAMEMSG*);

inline void drawCrossAt(const nuvec_s &pos, float size) {
  *debugCrossPos = pos;
  *debugSize = size;
  (*DrawCross)();
}
