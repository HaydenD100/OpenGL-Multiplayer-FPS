#pragma once

#include <GL/glew.h>
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Engine/Core/Renderer.h"


struct Wall {
	Wall() = default;
	Wall(glm::vec3 start, glm::vec3 end);
	void AddDoor(glm::vec3 position);

	glm::vec3(start);
	glm::vec3(end);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector <glm::vec3> normals;
	std::string texture_name;

	int height = 3;

};

struct Room {
	Room() = default;
	Room(glm::vec3 pos,int width, int lenght);

	void Render();
	int floor_height = 0;
	int width = 0;
	int lenght = 0;
	glm::vec3 position;
	std::vector<Wall> walls;

	//a wall just holds a quad
	glm::vec3 floor[6];

	GLuint vertexbuffer;
	std::vector<glm::vec3> vertices;



};

struct Level {
	Level() = default;
	std::vector<Room> rooms;
};


namespace EditorManager
{
	void Init();
	void Update();
};

