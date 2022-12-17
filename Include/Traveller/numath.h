#pragma once

struct nuvec_s {
  float x;
  float y;
  float z;
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
