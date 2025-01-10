#include "Raycaster.h"
#include "Engine/Core/AssetManager.h"




namespace Raycaster
{
	GLuint vertexBuffer = 0;
	GLuint indicesBuffer = 0;
	GLuint modelMatrixBuffer = 0;

	GLuint rayDirectionBuffer = 0;
	GLuint rayOriginBuffer = 0;
	GLuint rayLengthBuffer = 0;

	std::vector<glm::vec3> raysDirection;
	std::vector<glm::vec4> raysOrigin;
	std::vector<float> raysLength;

	int indicesSize = 0;


	void Init() {
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
		//TODO :: Dynamicaly calculate size
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100000 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

		glGenBuffers(1, &indicesBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100000 * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW); // 0 means no data, size is set later

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

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
	void FillBuffers() {
		size_t objectListSize = AssetManager::GetGameObjectsSize();
		int vertexBufferOffset = 0;
		int indicesBufferOffset = 0;
		indicesSize = 0;

		for (int i = 0; i < objectListSize; i++) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
			vertexBufferOffset += AssetManager::GetGameObject(i)->GetModel()->VertexInfoBind(vertexBufferOffset,i);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesBuffer);
			indicesBufferOffset += AssetManager::GetGameObject(i)->GetModel()->IndicesInfoBind(indicesBufferOffset);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, modelMatrixBuffer);
			glm::mat4 modelMatrix = AssetManager::GetGameObject(i)->GetModelMatrix();
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelMatrix));
		}
		indicesSize = indicesBufferOffset / sizeof(unsigned short);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void CleanUp() {

	}
	void Bind() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, vertexBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, indicesBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, modelMatrixBuffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayOriginBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysOrigin.size() * sizeof(glm::vec4), &raysOrigin[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, rayOriginBuffer);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayDirectionBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysDirection.size() * sizeof(glm::vec3), &raysDirection[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, rayDirectionBuffer);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayLengthBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, raysLength.size() * sizeof(float), &raysLength[0], GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, rayLengthBuffer);
	}

	void Compute() {
		std::cout << raysDirection.size() << "\n";
		glDispatchCompute(raysDirection.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

		//raysDirection.clear();
		//raysOrigin.clear();
		//raysLength.clear();
	}
	void queueRay(glm::vec3 rayDirection, glm::vec3 rayOrigin, float length, int probeID){
		raysDirection.push_back(rayDirection);
		raysOrigin.push_back(glm::vec4(rayOrigin, probeID));
		raysLength.push_back(length);
	}	
	int GetIndicesSize() {
		return indicesSize;
	}



}
