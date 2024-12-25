#include "StorageBuffer.h"



void StorageBuffer::Configure(int size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ID);
}
void StorageBuffer::Bind(int index) {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ID);

}


GLuint StorageBuffer::GetID() {
	return ID;
}
