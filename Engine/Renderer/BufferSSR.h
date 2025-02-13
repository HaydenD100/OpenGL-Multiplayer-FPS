#pragma once
#include "Engine/Core/Common/RenderCommon.h"

class BufferSSR
{
public:
	BufferSSR();
	~BufferSSR();

	void Bind();
	void Destroy();
	void Configure(int width, int height);
	unsigned int GetID();

	unsigned int gSSR, Depth;

private:
	const static int attachments =1;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0};
	unsigned int ID = 0;
};