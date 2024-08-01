#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Light {
	glm::vec3 position = glm::vec3(0,0,0);
	glm::vec3 colour = glm::vec3(1,1,1);
	float strength = 5.0f;

	Light(glm::vec3 position, glm::vec3 colour, float strength);
};
