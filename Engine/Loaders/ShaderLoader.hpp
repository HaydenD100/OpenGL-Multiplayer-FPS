#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>>
#include <sstream>
#include <vector>

namespace LoadShaders {
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
}
