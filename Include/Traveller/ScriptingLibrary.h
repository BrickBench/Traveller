#pragma once

#include <string>

#include "Mod.h"
#include "pch.h"

namespace ScriptingLibrary {
inline std::string currentModule;

TTSLLib void log(const std::string &str);

void init();

void onKeyboardInput(int message, int keyCode);

std::shared_ptr<BaseMod> getModByName(const std::string &name);
}; // namespace ScriptingLibrary
