#include "Backend.h"
#include "Engine/Core/UI/UI.h"
#include "Engine/Renderer/Renderer.h"
#include <iostream>
#include "Engine/Core/Camera.h"
#include "Engine/Core/UI/UI.h"



namespace Backend
{
	GLFWwindow* window;
	bool windowOpen = true;
	int width = DEFAULT_WIDTH;
	int height = DEFAULT_HEIGHT;

	void APIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* userParam) {
		std::cerr << "OpenGL Debug Message [" << id << "]: " << message << std::endl;

		if (id == 131218) { // Filter the specific message
				GLuint programId = 19; // Extract from the message
				if (glIsProgram(programId)) {
					GLint linkStatus;
					glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

					GLint numShaders;
					glGetProgramiv(programId, GL_ATTACHED_SHADERS, &numShaders);
					GLuint* shaders = new GLuint[numShaders];
					GLsizei count;
					glGetAttachedShaders(programId, numShaders, &count, shaders);

					for (int i = 0; i < count; ++i) {
						GLint type;
						glGetShaderiv(shaders[i], GL_SHADER_TYPE, &type);
						GLint sourceLen;
						glGetShaderiv(shaders[i], GL_SHADER_SOURCE_LENGTH, &sourceLen);
						char* source = new char[sourceLen];
						glGetShaderSource(shaders[i], sourceLen, NULL, source);
						std::cout << "Shader " << shaders[i] << " (Type: " << type << ")\nSource:\n" << source << std::endl;
						delete[] source;
					}
					delete[] shaders;
				}
		}

		// Optional: filter severity/type
		if (severity == GL_DEBUG_SEVERITY_HIGH) {
			std::cerr << "** GL DEBUG: HIGH SEVERITY ERROR **" << std::endl;
		}
	}

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
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Enable debug context


#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOWTITILE, nullptr, nullptr);


		if (window == nullptr) {
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window); // Initialize GLEW
		glewExperimental = true; // Needed in core profile
		if (glewInit() != GLEW_OK) {
			return -1;
		}


		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes debugging easier

			glDebugMessageCallback(GLDebugCallback, nullptr);

			// Optional: turn off notifications
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
				0, nullptr, GL_FALSE);
		}


		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	
		ImGuiUI::Init();
		Renderer::init();
		return 0;
	}

	GLFWwindow* Backend::GetWindowPointer() {
		if (window == nullptr)
			std::cout << "this is null \n";
		checkOpenGLError();

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

	void framebuffer_size_callback(GLFWwindow* window, int m_width, int m_height) {
		std::cout << "Window resized to: " << m_width << "x" << m_height << std::endl;
		height = m_height;
		width = m_width;
		glViewport(0, 0, width, height);
		Renderer::ConfigureFrameBuffers();
		Camera::RecalcuteProjectionMatrix();
	}
}
