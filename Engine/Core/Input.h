#pragma once
#include "Engine/Core/Common/Header.h"
#include <glm/glm.hpp>
#include "Engine/Backend.h"
#include <iostream>
#include <cctype>




namespace Input
{
	void Init();
	void Update();

	int GetMouseX();
	int GetMouseY();
	bool LeftShiftDown();
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

	float GetSensitivity();
	void SetSensitivity(float sensitivity);


	void windowFocusCallback(GLFWwindow* window, int focused);
}
