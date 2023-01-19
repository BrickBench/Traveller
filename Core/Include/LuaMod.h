
#pragma once
#include <map>
#include <sol/state.hpp>

#include "Configuration.h"
#include "Mod.h"

class LuaMod : public Mod {
private:
  std::map<std::string, std::unique_ptr<sol::table>> loadedScripts;

  void runScript(const std::string &name, const std::string &func);

public:
  inline static bool useFennelInterpreter = false;

  std::string getName() override { return "LuaScript"; }

  Configuration::ConfigurationData getDefaultConfiguration() override;

  void earlyInit() override;

  void lateInit() override;

  void earlyUpdate(double delta) override;

  void earlyRender() override;

  void lateRender() override;

  void execScript(const std::string &script);
};
