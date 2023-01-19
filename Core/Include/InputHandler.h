#pragma once
#include <cstddef>
#include <imgui.h>

namespace InputHandler {
std::byte *getCurrentKeyTable();

bool readKey(int key);

ImVec2 getMousePos();

void requestMouseAccess(bool access);

void updateInputs();
}; // namespace InputHandler
