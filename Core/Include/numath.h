#pragma once
#include "nuutil.h"

struct nuvec_s {
  float x;
  float y;
  float z;

  bool operator==(const nuvec_s& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct nuvec4_s_ {
  float x;
  float y;
  float z;
  float w;
};

struct numtx_s {
  nuvec4_s_ r0;
  nuvec4_s_ r1;
  nuvec4_s_ r2;
  nuvec4_s_ r3;
};

TRAVELLER_REGISTER_RAW_GLOBAL(0x0095e5f4, int, _fseed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x008102e8, int, _partSeed_);
TRAVELLER_REGISTER_RAW_GLOBAL(0x0080253c, int, _qseed);
TRAVELLER_REGISTER_RAW_GLOBAL(0x00810900, int, _debrisSeed_);
