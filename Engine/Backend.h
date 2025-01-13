#pragma once


//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Engine/Core/Common.h"
#include "Engine/Renderer/Renderer.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



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
