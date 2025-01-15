#include "Backend.h"

#include "Engine/Core/UI/UI.h"



namespace Backend
{
	GLFWwindow* window;
	bool windowOpen = true;
	int width = 1280;
	int height = 720;



	int Backend::init() {
		glewExperimental = true; // Needed for core profile
		if (!glfwInit())
		{
			return -1;
		}

		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(width, height , WINDOWTITILE, nullptr, nullptr);

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
	int GetHeight() {
		return height;
	}
	int GetWidth() {
		return width;
	}
	void SetWindowResolution(int Width, int Height) {
		width = Width;
		height = Height;
	}
}
