#pragma once
#include <map>
#include <sol/state.hpp>

#include "Mod.h"

class CoreMod : public BaseMod
{
private:
	std::map<std::string, std::unique_ptr<BaseMod>> loadedMods;
	std::map<std::string, std::unique_ptr<sol::table>> loadedScripts;


	void runScript(const std::string& name, const std::string& func);

public:
	std::string getName() override
	{
		return "CoreMod";
	}

	void earlyInit() override;

	void lateInit() override;

	void earlyUpdate(double delta) override;

	void earlyRender() override;

	void lateRender() override;

	void drawUI() override;
};

