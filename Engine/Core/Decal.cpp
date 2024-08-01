#include "Decal.h"

Decal::Decal(glm::vec3 position, glm::vec3 normal, glm::vec3 scale, Texture* texture, GameObject* Parent) {
    this->texture = texture;
	this->normal = normal;
	parent = Parent;

	glm::vec3 up(0, 1, 0);
	glm::vec3 rotationAxis = glm::cross(normal, up);
	float angle = acos(glm::dot(normal, up));

	transform.position = position;
	transform.rotation = (rotationAxis * -angle);
	std::cout << "debug" << std::endl;
	std::cout << angle << std::endl;
	std::cout << "x: " << normal.x << " y: " << position.y << " z: " << position.z << std::endl;
	std::cout << "x: " << transform.rotation.x << " y: " << transform.rotation.y << " z: " << transform.rotation.z << std::endl;

	transform.scale = scale;

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
}

bool Decal::CheckParentIsNull() {
	if (parent == nullptr)
		return true;
	return false;
}

glm::mat4 Decal::GetModel() {
	return parent->GetModelMatrix() * transform.to_mat4();
}

glm::vec3 Decal::GetNormal() {
	return normal;
}

void Decal::RenderDecal(GLuint& programID) {
	glUseProgram(programID);

	if (texture != nullptr) //< Should this be an assert instead? Do we want to continue??
	{
		glActiveTexture(texture->GetTextureNumber() + GL_TEXTURE0);
		GLuint TextureID = glGetUniformLocation(programID, "DiffuseTextureSampler");
		glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
		glUniform1i(TextureID, texture->GetTextureNumber());

		glActiveTexture(texture->GetTextureNormalNumber() + GL_TEXTURE0);
		GLuint NormalID = glGetUniformLocation(programID, "NormalTextureSampler");
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureNormal());
		glUniform1i(NormalID, texture->GetTextureNormalNumber());
	}
	
	// 1st attribute buffer : vertices
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

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	
	// 4th attribute buffer : tangents
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 5th attribute buffer : bitangents
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glVertexAttribPointer(
		4,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}
