#pragma once
#include <map>
#include <sol/state.hpp>

#include "Configuration.h"
#include "Mod.h"

class CoreMod : public BaseMod {
private:
  std::map<std::string, std::unique_ptr<sol::table>> loadedScripts;

  void runScript(const std::string &name, const std::string &func);

  void applyWindowChangeHooks();

public:
  std::shared_ptr<Configuration> coreConfig;

  inline static bool useFennelInterpreter = false;

  std::string getName() override { return "CoreMod"; }

  void earlyInit() override;

  void lateInit() override;

  void earlyUpdate(double delta) override;

  void earlyRender() override;

  void lateRender() override;

  void onKeyboardInput(int message, int keyCode) override;

  static void execScript(const std::string &script);
};
