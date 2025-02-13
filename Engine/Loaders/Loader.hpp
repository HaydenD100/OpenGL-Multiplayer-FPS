#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bit>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Engine/Core/Common/Header.h"


#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace Loader
{
	GLuint loadBMP_custom(const char* imagepath);
	GLuint loadDDS(const char* imagepath);
	bool loadOBJ(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals);
}
