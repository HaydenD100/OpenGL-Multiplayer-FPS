#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"


static const int spawnpointsize = 1;
static glm::vec3 spawnpoints[spawnpointsize] = { glm::vec3(20, 100, 10) };


//Player Data
#define SWIMMINGMOD  0.9
#define WALKINGSPEED  4000
#define CRROUCHINGSPEED  2000
#define RUNNINGSPEED  6500
#define JUMPFORCE 6
#define MAXSPEED 4


