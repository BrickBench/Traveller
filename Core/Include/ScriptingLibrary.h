#pragma once

#include <string>

#include "Mod.h"
#include "pch.h"

namespace ScriptingLibrary {
inline std::string currentModule;

TTSLLib void log(const std::string &str);

TTSLLib void init();

TTSLLib void onKeyboardInput(int message, int keyCode);

TTSLLib std::shared_ptr<Mod> getModByName(const std::string &name);
}; // namespace ScriptingLibrary
