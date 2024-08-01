#pragma once
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Core/AssetManager.h"
#include "Engine/Physics/Physics.h"

class Window
{
public:
	Window(std::string name, const char* frame_path, Texture* frame_texture, const char* window_path, Texture* window_texture, glm::vec3 position, glm::vec3 rotation);
	void Render(GLuint programID, glm::mat4 View, glm::mat4 projection);
private:
	GameObject window;
	std::string name;
};