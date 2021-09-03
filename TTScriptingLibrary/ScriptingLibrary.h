#pragma once

#include <string>

#include "pch.h"

namespace ScriptingLibrary
{
	inline std::string currentModule;

	TTSLLib void log(const std::string& str);

	void init();
};

