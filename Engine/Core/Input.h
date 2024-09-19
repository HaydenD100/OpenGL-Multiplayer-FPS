#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Engine/Backend.h"
#include "Engine/Core/Common.h"
#include <iostream>
#include <cctype>

namespace Input
{
	void Init();
	void Update();

	int GetMouseX();
	int GetMouseY();
	bool KeyPressed(char c);
	bool KeyDown(char c);
	bool KeyDownLastFrame(char c);
	void CenterMouse();
	bool LeftMouseDown();
	bool RightMouseDown();
	bool LeftMousePressed();
	bool RightMousePressed();
	float GetMouseOffsetX();
	float GetMouseOffsetY();

	void DisableCursor();
	void HideCursor();
	void ShowCursor();
}
