#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define MAX_LIGHTS 100
#define MAX_PARTICLES 1000

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define UPDATED_PROBE_COUNT_PER_FRAME 200


const glm::vec3 DEFAULT_SKY_COLOR(0.1, 0.1, 0.1);
const int PROBESIZE = 16;
const int DDGIPROBESIZE = 16;



enum LightType
{
	PointLight,
	Spotlight,
	DirectionalLight,
};