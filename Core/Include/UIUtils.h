#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "numath.h"
#include "pch.h"
#include <string>

class TTSLLib UIUtils {
public:
  static bool InputNuVec(std::string label, nuvec_s& outVec) {
    return ImGui::InputFloat3(label.c_str(), reinterpret_cast<float*>(&outVec));
  }
};

