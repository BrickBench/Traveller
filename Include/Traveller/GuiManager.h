#pragma once

#include <string>
#include <vector>

namespace Gui
{
	void startRender();

	void endRender();

	void initializeImGui();

	void writeToConsole(const std::string& value);
};

