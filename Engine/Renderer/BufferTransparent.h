#pragma once
#include "Engine/Core/Common/RenderCommon.h"


class BufferTransparent
{
public:
	BufferTransparent();
	~BufferTransparent();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();

	unsigned int gLighting, gData, Depth;

private:
	const static int attachments = 2;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	unsigned int ID = 0;
};