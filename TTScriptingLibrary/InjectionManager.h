#pragma once

#include <iostream>

#include "MemWriteUtils.h"
#include "MinHook.h"
namespace InjectionManager
{
	inline void initialize()
	{
		if (MH_Initialize() != MH_OK)
		{
			std::cout << "Failed to initialize hook library" << std::endl;
		}
	}

	template <void(*F)(), void* TargetFunc, bool CallPost>
	void callTemplate()
	{
		typedef void (*fptr)();
		auto oldFunc = reinterpret_cast<fptr>(reinterpret_cast<long>(TargetFunc));

		if constexpr(CallPost)
		{
			(*F)();
			(*oldFunc)();

		}
		else
		{
			(*F)();
			(*oldFunc)();
		}
	}

	template <void(*NewFunc)(), int CallInstr, void* TargetFunc, bool CallPost>
	TTSLLib void injectFunction()
	{
		constexpr int replacementOffset = CallInstr + 1;
		auto templateFuncPtr = &callTemplate<NewFunc, TargetFunc, CallPost>;

		int reljmp = reinterpret_cast<int>(templateFuncPtr) - (CallInstr + 5);

		auto last = MemWriteUtils::setMemoryPerms(CallInstr, 8, PAGE_EXECUTE_READWRITE);
		*reinterpret_cast<int*>(replacementOffset) = reljmp;
		MemWriteUtils::setMemoryPerms(CallInstr, 8, last);
	}

	TTSLLib inline void replaceDllFunction(const std::string& dllName, const std::string& functionName, void* newFunction)
	{
		std::wstring dllwName = std::wstring(dllName.begin(), dllName.end());
		auto result = MH_CreateHookApi(dllwName.c_str(), functionName.c_str(), newFunction, nullptr);

		if (result != MH_OK)
		{
			std::cout << "Failed to disable " << dllName << "/" << functionName << ": " << MH_StatusToString(result) << std::endl;
		}

		MH_EnableHook(MH_ALL_HOOKS);
	}
};
