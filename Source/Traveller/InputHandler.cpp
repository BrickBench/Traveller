#include "InputHandler.h"

#include "pch.h"
#include <cstddef>
#include <imgui.h>
#include <iostream>

int getInputRemapperAddress()
{
	return *reinterpret_cast<int*>(0x02975164);
}

void disableKeyInputs()
{
	auto keys = InputHandler::getCurrentKeyTable();

	for (int i = 0; i < 0x100; i++)
	{
		keys[i] = static_cast<std::byte>(0);
	}
}

std::byte* InputHandler::getCurrentKeyTable()
{
	auto currentKeyTable = static_cast<int>(*reinterpret_cast<std::byte*>(0x029755c1));
	auto keyBuffer = reinterpret_cast<std::byte*>(0x029751c0);

	return keyBuffer + (currentKeyTable * 0x100);
}

bool InputHandler::readKey(int key)
{
	return *(getCurrentKeyTable() + key) != static_cast<std::byte>(0);
}

ImVec2 InputHandler::getMousePos()
{
	auto xPos = *reinterpret_cast<float*>(getInputRemapperAddress() + 0x632c);
	auto yPos = *reinterpret_cast<float*>(getInputRemapperAddress() + 0x6330);

	return ImVec2(xPos, yPos);
}

void InputHandler::requestMouseAccess(bool access)
{
	*reinterpret_cast<int*>(0x02976740) = access ? 1 : 0;
}

void InputHandler::updateInputs()
{
}
