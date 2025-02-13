#pragma once
#include "Engine/Core/Common/Header.h"




namespace Backend
{
	int init();
	GLFWwindow* GetWindowPointer();
	bool IsWindowOpen();
	void CloseWindow();

	int GetHeight();
	int GetWidth();
	void SetWindowResolution(int width, int height);

}
