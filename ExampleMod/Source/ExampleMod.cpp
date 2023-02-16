#include "pch.h"

#include <imgui.h>

#include "Mod.h"
#include "Traveller.h"

class ExampleMod : public Mod {
  std::string getName() override { return "ExampleMod"; }

public:
  void earlyInit() override { Traveller::log("I'm here!"); };
};

extern "C" __declspec(dllexport) Mod *getModInstance() { return new ExampleMod(); }
