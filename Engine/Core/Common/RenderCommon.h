#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define MAX_LIGHTS 100

#define DEFAULT_WIDTH 960//832
#define DEFAULT_HEIGHT 540//468


const int PROBESIZE = 64;
const int DDGIPROBESIZE = 16;

enum LightType
{
	PointLight,
	Spotlight,
	DirectionalLight,
};