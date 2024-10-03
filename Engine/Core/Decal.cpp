#include "Decal.h"
#include "DecalInstance.h"


Decal::Decal(std::string name, Texture* texture, glm::vec3 size) {
    this->texture = texture;
	this->name = name;
	this->size = size;

	glGenBuffers(1, &instanceBuffer);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

size_t Decal::getVerticesSize() {
	return indexed_vertices.size();
}
std::vector<glm::vec3> Decal::getVerticies() {
	return indexed_vertices;
}
glm::vec3 Decal::GetVertex(int i) {
	return indexed_vertices[i];
}
std::string Decal::GetName() {
	return name;
}
void Decal::AddInstace(DecalInstance* instance) {
	instancesModel.push_back(instance->GetModel());
}
void Decal::ClearInstace() {
	instancesModel.clear();
}
glm::vec3 Decal::GetSize() {
	return size;
}




void Decal::RenderDecal(GLuint& programID) {
	if (instancesModel.size() <= 0)
		return;
	glUseProgram(programID);

	if (texture != nullptr) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, texture->GetTextureNormal());
	}	
	
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

	
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, instancesModel.size() * sizeof(glm::mat4), &instancesModel[0], GL_STATIC_DRAW);
	// Set up the instance matrix attributes
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(1 + i);
		glVertexAttribPointer(
			1 + i,              // Attribute location
			4,                  // Size (4 components per vec4)
			GL_FLOAT,           // Type
			GL_FALSE,           // Normalized?
			sizeof(glm::mat4),  // Stride
			(void*)(sizeof(float) * i * 4)  // Offset for each column of the matrix
		);
		glVertexAttribDivisor(1 + i, 1);  // Per-instance data
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles with instancing
	glDrawElementsInstanced(
		GL_TRIANGLES,        // mode
		indices.size(),      // count
		GL_UNSIGNED_SHORT,    // type
		(void*)0,            // element array buffer offset
		instancesModel.size()  // Number of instances
	);


	// Optionally unbind the buffers
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// After drawing
	for (int i = 0; i < 4; i++) {
		glVertexAttribDivisor(1 + i, 0);  // Reset to per-vertex
	}

	instancesModel.clear();
}

