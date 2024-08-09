#include "EditorManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Backend.h"
#include "Engine/Core/Input.h"
namespace EditorManager
{

	float fov = glm::radians(180.0f); // Field of view in radians
	float aspectRatio = SCREENHEIGHT / SCREENHEIGHT; // Aspect ratio (width/height)
	float nearClip = 0.0f; // Near clipping plane
	float farClip = 100.0f; // Far clipping plane

	glm::mat4 projection = glm::perspective(fov, aspectRatio, nearClip, farClip);
	glm::mat4 ViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(0.0f, 10.0f, 0.0f); // Start position above the scene
	glm::vec3 targetPosition = glm::vec3(0.0f, 1.0f, 0.0f); // Look at the point directly below

	

	Room room(glm::vec3(0, 0, 0), 1, 1);

	void EditorManager::Init() {
		
	}
	void EditorManager::Update() {
		if (Input::KeyDown('w'))
			cameraPosition.x += 0.005;
		if (Input::KeyDown('s'))
			cameraPosition.x -= 0.005;
		if (Input::KeyDown('a'))
			cameraPosition.z += 0.005;
		if (Input::KeyDown('d'))
			cameraPosition.z -= 0.005;


		//std::cout << cameraPosition.x << " y: " << cameraPosition.y << " z: " << cameraPosition.z << std::endl;


		Renderer::UseProgram(Renderer::GetProgramID("editor"));

		targetPosition = glm::vec3(cameraPosition.x, 0.0f, cameraPosition.z);

		ViewMatrix = glm::lookAt(cameraPosition, targetPosition, glm::vec3(0,1,0));


		GLuint projectionLoc = glGetUniformLocation(Renderer::GetCurrentProgramID(), "projection");
		GLuint viewLoc = glGetUniformLocation(Renderer::GetCurrentProgramID(), "view");

		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &ViewMatrix[0][0]);


		room.Render();

	}
}


Wall::Wall(glm::vec3 start, glm::vec3 end) {
	
	vertices.push_back(start + glm::vec3(0,height,0));
	vertices.push_back(start);
	vertices.push_back(end);

	vertices.push_back(start + glm::vec3(0, height, 0));
	vertices.push_back(end + glm::vec3(0, height, 0));
	vertices.push_back(end);

}
Room::Room(glm::vec3 pos, int width, int length) {
	

	this->lenght = length;
	this->width = width;
	position = pos;

	glm::vec3 halfWidth = glm::vec3(width / 2.0f, 0.0f, 0.0f);
	glm::vec3 halfLength = glm::vec3(0.0f, 0.0f, length / 2.0f);

	glm::vec3 topLeft = pos - halfWidth + halfLength;
	glm::vec3 topRight = pos + halfWidth + halfLength;
	glm::vec3 bottomLeft = pos - halfWidth - halfLength;
	glm::vec3 bottomRight = pos + halfWidth - halfLength;
	//north
	walls.push_back(Wall(topLeft,topRight));
	//east
	walls.push_back(Wall(topRight,bottomRight));
	//south
	walls.push_back(Wall(bottomLeft, bottomRight));
	//west
	walls.push_back(Wall(topLeft, bottomLeft));

	floor[0] = topLeft;
	floor[1] = bottomLeft;
	floor[2] = bottomRight;
	floor[3] = topLeft;
	floor[4] = topRight;
	floor[5] = bottomRight;



}
//i know this is stupid its just for testing
void Room::Render() {

	vertices.clear();

	for (int i = 0; i < 6; i++) {
		vertices.push_back(floor[i]);
	}

	for (int i = 0; i < walls.size(); i++) {
		vertices.insert(std::end(vertices), std::begin(walls[i].vertices), std::end(walls[i].vertices));
	}

	GLuint modelLoc = glGetUniformLocation(Renderer::GetCurrentProgramID(), "model");
	glm::mat4 m = glm::translate(glm::mat4(1), glm::vec3(1,0,1));
	m *= glm::mat4_cast(glm::quat(glm::vec3(0,0,0)));
	m = glm::scale(m, glm::vec3(1,1,1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m[0][0]);




	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
