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

	unsigned int gLighting, gData, gPosition, Depth;

private:
	const static int attachments = 3;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 };
	unsigned int ID = 0;
};