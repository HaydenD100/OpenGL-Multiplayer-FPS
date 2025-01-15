#include "BufferLighting.h"

BufferLighting::BufferLighting() {

}

BufferLighting::~BufferLighting() {

}

void BufferLighting::Configure() {
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glGenTextures(1, &gLighting);
	glBindTexture(GL_TEXTURE_2D, gLighting);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Backend::GetWidth(), Backend::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	

	glGenTextures(1, &Depth);
	glBindTexture(GL_TEXTURE_2D, Depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, Backend::GetWidth(), Backend::GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Depth, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLighting, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete!" << std::endl;
	}
	// Set the list of draw buffers.
	glDrawBuffers(attachments, DrawBuffers);
}

void BufferLighting::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}
void BufferLighting::Destroy() {

}
unsigned int BufferLighting::GetID() {
	return ID;
}