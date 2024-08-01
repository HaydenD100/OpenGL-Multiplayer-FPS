#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Loaders/loader.hpp"
#include "Loaders/ShaderLoader.hpp"
#include "Engine/Core/Renderer.h"

namespace Text2D {
	void initText2D(const char* texturePath);
	void printText2D(const char* text, int x, int y, int size);
	void cleanupText2D();
	GLuint GetProgramID();
}
