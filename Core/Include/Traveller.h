#pragma once

#include <string>
#include <map>

#include "Mod.h"
#include "pch.h"

namespace Traveller {
inline std::string currentModule;

TTSLLib void log(const std::string &str);

TTSLLib void init();

TTSLLib void onKeyboardInput(int message, int keyCode);

TTSLLib std::shared_ptr<Mod> getModByName(const std::string &name);

TTSLLib std::map<std::string, std::shared_ptr<Mod>> getMods();
}; // namespace Traveller
