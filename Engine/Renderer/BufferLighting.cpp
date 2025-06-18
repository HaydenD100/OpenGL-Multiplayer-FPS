#include "BufferLighting.h"
#include "Engine/Backend.h"
#include <iostream>
BufferLighting::BufferLighting() {

}

BufferLighting::~BufferLighting() {
	Destroy();
}

void BufferLighting::Configure() {
	glGenTextures(1, &gLighting);
	glBindTexture(GL_TEXTURE_2D, gLighting);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Backend::GetWidth(), Backend::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void BufferLighting::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}
void BufferLighting::Destroy() {
	glDeleteTextures(1, &gLighting);
}
unsigned int BufferLighting::GetID() {
	return ID;
}
