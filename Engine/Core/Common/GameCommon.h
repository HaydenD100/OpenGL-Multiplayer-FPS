#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"

static const int spawnpointsize = 1;
static glm::vec3 spawnpoints[spawnpointsize] = { glm::vec3(0, 20, 4) };


//Player Data
#define WALKINGSPEED  6000
#define CRROUCHINGSPEED  3000
#define RUNNINGSPEED  8500
#define JUMPFORCE 9
#define MAXSPEED 6


