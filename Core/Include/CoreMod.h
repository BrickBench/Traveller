#pragma once
#include <map>

#include "Configuration.h"
#include "Mod.h"

class CoreMod : public Mod {
private:
  void applyWindowChangeHooks();

public:
  std::string getName() override { return "Core"; }

  Configuration::ConfigurationData getDefaultConfiguration() override;

  void earlyInit() override;

  void lateInit() override;

  void lateRender() override;

  void drawUI() override;

  void onKeyboardInput(int message, int keyCode) override;
};
