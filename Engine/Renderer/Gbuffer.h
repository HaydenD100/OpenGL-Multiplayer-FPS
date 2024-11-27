#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Engine/Backend.h"

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();
	
	unsigned int gAlbedo, gNormal, gPosition, gRMA, Depth;

private:
	const static int attachments = 4;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	unsigned int ID = 0;
};