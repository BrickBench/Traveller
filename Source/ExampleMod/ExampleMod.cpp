#include "pch.h"

#include <imgui.h>

#include "Mod.h"
#include "ScriptingLibrary.h"

class ExampleMod : public BaseMod {
  std::string getName() override { return "ExampleMod"; }

public:
  void earlyInit() override { ScriptingLibrary::log("I'm here!"); };
};

extern "C" BaseMod *getModInstance() { return new ExampleMod(); }
