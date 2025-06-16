#include "Raycaster.h"
#include "Engine/Core/AssetManager.h"
#include "glm/gtx/intersect.hpp"

#include "Engine/Core/Scene/World.h"

namespace Raycaster
{
	GLuint vertexBuffer = 0;
	GLuint indicesBuffer = 0;
	GLuint modelMatrixBuffer = 0;

	GLuint rayDirectionBuffer = 0;
	GLuint rayOriginBuffer = 0;
	GLuint rayLengthBuffer = 0;
	GLuint rayInfoBuffer = 0;


	std::vector<glm::vec3> raysDirection;
	std::vector<glm::vec3> raysOrigin;
	std::vector<glm::vec4> raysInfo;

	std::vector<float> raysLength;

	std::vector<glm::vec4> verticies;


	unsigned int vertexBufferDebug;

	int indicesSize = 0;


	void Init() {

		glGenBuffers(1, &modelMatrixBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatrixBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &rayDirectionBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayDirectionBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

		glGenBuffers(1, &rayOriginBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayOriginBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

		glGenBuffers(1, &rayLengthBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayLengthBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

		glGenBuffers(1, &rayInfoBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayInfoBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
	void FillBuffers() {
		size_t objectListSize = World::g_objects.size();	
		verticies.clear();

		for (int i = 0; i < objectListSize; i++) {
			GameObject* object = World::g_objects[i].get();
			//for now just keep tbis the map as it only has a few verticies
			if (!object->IncludedInRayCast())
				continue;

			std::vector<glm::vec4> verticesModel = object->GetModel()->GetVerticiesPadded();
			std::cout << verticesModel.size() << "\n";
			copy(verticesModel.begin(), verticesModel.end(), back_inserter(verticies));

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatrixBuffer);
			glm::mat4 modelMatrix = object->GetModelMatrix();
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelMatrix));
		}
		std::cout << "verticies size GPU" << verticies.size() << "\n";

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, verticies.size() * sizeof(glm::vec4), &verticies[0], GL_STATIC_DRAW);
	}

	void CleanUp() {

	}
	void Bind() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, vertexBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, modelMatrixBuffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayOriginBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysOrigin.size() * sizeof(glm::vec3), &raysOrigin[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, rayOriginBuffer);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayDirectionBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysDirection.size() * sizeof(glm::vec3), &raysDirection[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, rayDirectionBuffer);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayLengthBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysLength.size() * sizeof(float), &raysLength[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, rayLengthBuffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayInfoBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysInfo.size() * sizeof(glm::vec4), &raysInfo[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 14, rayInfoBuffer);
	}

	void Compute() {

		Renderer::cs_Raycaster.Use();
		Bind();
		Renderer::cs_Raycaster.SetInt("verticiesSize", verticies.size());
		glDispatchCompute(raysDirection.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

		//raysDirection.clear();
		//raysOrigin.clear();
		//raysLength.clear();
	}
	void queueRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float length, glm::vec4 rayInfo) {
		raysDirection.push_back(rayDirection);
		raysOrigin.push_back(glm::vec4(rayOrigin, 0));
		raysLength.push_back(length);
		raysInfo.push_back(rayInfo);
	}	

	int GetIndicesSize() {
		return indicesSize;
	}
	void RenderVerticies() {
		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, verticies.size());

		//glDisableVertexAttribArray(0);  // Cleanup if necessary
		//glBindVertexArray(0);  // Unbind VAO
	}



}

struct IntersectionResult {
	bool hitFound = false;
	float distance = 0;
	float dot = 0;
	glm::vec2 baryPosition = glm::vec2(0);
};


namespace SoftwareRaycaster
{
	std::vector<glm::vec3> raysDirection;
	std::vector<glm::vec3> raysOrigin;
	std::vector<float> raysLength;

	std::vector<glm::vec3> verticies;


	//debug
	unsigned int vertexBuffer;

	void Init() {

	}
	void FillBuffers() {
		verticies.clear();
		for (int i = 0; i < World::g_objects.size(); i++) {
			GameObject* object = World::g_objects[i].get();
			//for now just keep tbis the map as it only has a few verticies
			if (!object->IncludedInRayCast())
				continue;

			std::vector<glm::vec3> verticesModel = object->GetModel()->GetVerticies();
			std::cout << verticesModel.size() << "\n";
			copy(verticesModel.begin(), verticesModel.end(), back_inserter(verticies));
		}
		std::cout << "verticies size SOFTWARE " << verticies.size() << "\n";

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);
	}
	void Compute() {
		std::cout << raysDirection.size() << "\n";

		for (int ray = 0; ray < raysDirection.size(); ray++) {
			float maxLength = 999999;
			float closestHit = maxLength;
			for (int i = 0; i < verticies.size(); i += 3) {
				glm::vec3 v0 = verticies[i];
				glm::vec3 v1 = verticies[i + 1];
				glm::vec3 v2 = verticies[i + 2];

				float result = TriangleIntersectionTest(v0, v1, v2, ray);

				if (result == -1)
					continue;

				if (result < closestHit && result > 0)
					closestHit = result;
			}
			if (closestHit != maxLength) {
				glm::vec3 cartesian = raysOrigin[ray] + raysDirection[ray] * closestHit;
				std::cout << "x " << cartesian.x << " y " << cartesian.y << " z " << cartesian.z << " \n";
				//World::GetGameObject("raytest")->setPosition(cartesian);
			}
			else {
				std::cout << "No Hit \n";
			}
			
		}
		

		raysDirection.clear();
		raysOrigin.clear();
		raysLength.clear();
	}
	void queueRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float length) {
		raysDirection.push_back(rayDirection);
		raysOrigin.push_back(rayOrigin);
		raysLength.push_back(length);


		std::cout << "------------RayInfo----------------- \n";
		std::cout << "Origin X: " << rayOrigin.x << " Y: " << rayOrigin.y << " Z: " << rayOrigin.z << "\n";
		std::cout << "Direction X: " << rayDirection.x << " Y: " << rayDirection.y << " Z: " << rayDirection.z << "\n";

	}



	

	float TriangleIntersectionTest(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int rayIndex) {
		glm::vec3 origin = raysOrigin[rayIndex];
		glm::vec3 direction = raysDirection[rayIndex];
		float lenght = raysLength[rayIndex];
		glm::vec3 result;
		
		//glm::intersectRayTriangle(origin, direction, p1, p2, p3, result);
		TriangleIntersectionTest2(origin, direction, p1, p2, p3, result);

		if (result.z == 0 || result.z > lenght)
			return -1;
		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		return result.z;
	}
	float TriangleIntersectionTest2(const glm::vec3& orig, const glm::vec3& dir,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		glm::vec3& baryPosition) {

		glm::vec3 e1 = v1 - v0;
		glm::vec3 e2 = v2 - v0;

		glm::vec3 p = glm::cross(dir, e2);
		float a = glm::dot(e1, p);

		const float epsilon = 0.0001;
		if (a > -epsilon && a < epsilon)
			return false;

		float f = 1.0f / a;
		glm::vec3 s = orig - v0;

		baryPosition.x = f * glm::dot(s, p);
		if (baryPosition.x < 0.0f || baryPosition.x > 1.0f)
			return false;

		glm::vec3 q = glm::cross(s, e1);
		baryPosition.y = f * glm::dot(dir, q);
		if (baryPosition.y < 0.0f || (baryPosition.x + baryPosition.y) > 1.0f)
			return false;

		baryPosition.z = f * glm::dot(e2, q);
		return baryPosition.z >= 0.0f;
	}

	void RenderVerticies() {
		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, verticies.size());

		//glDisableVertexAttribArray(0);  // Cleanup if necessary
		//glBindVertexArray(0);  // Unbind VAO
	}
};

