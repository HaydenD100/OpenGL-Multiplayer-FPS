#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Engine/Backend.h"

class BufferSSR
{
public:
	BufferSSR();
	~BufferSSR();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();

	unsigned int gSSR, Depth;

private:
	const static int attachments =1;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0};
	unsigned int ID = 0;
};