#include "Backend.h"

namespace Backend
{
	GLFWwindow* window;
	bool windowOpen = true;

	int Backend::init() {
		glewExperimental = true; // Needed for core profile
		if (!glfwInit())
		{
			return -1;
		}

		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT , WINDOWTITILE, nullptr, nullptr);

		if (window == nullptr) {
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window); // Initialize GLEW
		glewExperimental = true; // Needed in core profile
		if (glewInit() != GLEW_OK) {
			return -1;
		}

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Renderer::init();
		return 0;
	}

	GLFWwindow* Backend::GetWindowPointer() {
		return window;
	}

	bool Backend::IsWindowOpen() {
		return (!glfwWindowShouldClose(window) && windowOpen);
	}

	void Backend::CloseWindow() {
		windowOpen = false;
	}
}
