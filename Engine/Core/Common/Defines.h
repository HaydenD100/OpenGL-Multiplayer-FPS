#pragma once
#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#define SHADOW_MAP_SIZE 1024
#define SHADOW_NEAR_PLANE 0.05f
#define SHADOW_FAR_PLANE 30.0f
#define ENGINE_PI 3.141592653589793238462643

#define NRM_X_FORWARD glm::vec3(1,0,0)
#define NRM_X_BACK glm::vec3(-1,0,0)
#define NRM_Y_UP glm::vec3(0,1,0)
#define NRM_Y_DOWN glm::vec3(0,-1,0)
#define NRM_Z_FORWARD glm::vec3(0,0,1)
#define NRM_Z_BACK glm::vec3(0,0,-1)

#define ToRadian(x) (float)(((x) * ENGINE_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / ENGINE_PI))

#define NEAR_PLANE 0.0025f
#define FAR_PLANE 200.0f

#define WINDOWTITILE "Engine 2.0"
#define MAXDECALS 3000

#define MOUSE_SENSITIVITY = 0.002f;

#define RED glm::vec3(1,0,0)
#define GREEN glm::vec3(0,1,0)
#define BLUE glm::vec3(0,0,1)
#define YELLOW glm::vec3(1,1,0)
#define PURPLE glm::vec3(1,0,1)
#define CYAN glm::vec3(0,1,1)


enum Controls {
	RESPAWN = 'p',
	PLAYERINFO = 'o',
	FORWARD = 'w',
	LEFT = 'a',
	RIGHT = 'd',
	BACKWARD = 's',
	DROPWEAPON = 'q',
	JUMP = ' ',
	CROUCH = 'c',
	RELOAD = 'r',
	INTERACT = 'e',
	SPRAYPAINT = 'g',
	CYCLESPRAY = 't',
	RELOADSHADERS = 'h',
	BAKEGI= 'j',
	PROBETOGGLE = 'y',
	GUITOGGLE = 'u',
	EDITORTOGGLE = 'l',
	SPAWNFLOATIES = 'n',
	SHOWDEBUGTRIGGERS = 'k'
};