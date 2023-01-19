#pragma once

#include <string>
#include <vector>

namespace Gui {
void startRender(int width, int height);

void endRender();

void initializeImGui();

void writeToConsole(const std::string &value);
}; // namespace Gui
