#include "pch.h"

#include <imgui.h>

#include "Mod.h"
#include "ScriptingLibrary.h"

class ExampleMod : public Mod {
  std::string getName() override { return "ExampleMod"; }

public:
  void earlyInit() override { ScriptingLibrary::log("I'm here!"); };
};

extern "C" Mod *getModInstance() { return new ExampleMod(); }
