#include "input.h"

global_variable inputHandler_t inputHandler;

void SetScanCodeUp(uint32_t  KeyScanCode)
{
	inputHandler.keyStates[KeyScanCode].keyUp = true;
	inputHandler.keyStates[KeyScanCode].keyDown = false;
}

void SetScanCodeDown(uint32_t  KeyScanCode)
{
	inputHandler.keyStates[KeyScanCode].keyUp = false;
	inputHandler.keyStates[KeyScanCode].keyDown = true;
}

bool KeyDown(uint32_t  KeyScanCode)
{
	if (inputHandler.keyStates[KeyScanCode].keyDown)
	{
		return true;
	}
	return false;
}

bool KeyUp(uint32_t  KeyScanCode)
{
	if (inputHandler.keyStates[KeyScanCode].keyUp)
	{
		inputHandler.keyStates[KeyScanCode].keyUp = false;
		return true;
	}
	return false;
}

void SetMouse(uint32_t  lParamX, uint32_t  lParamY)
{
	inputHandler.mousePos.x = lParamX;
	inputHandler.mousePos.y = lParamY;
}

