#pragma once
#include "Engine/Core/Common/RenderCommon.h"

class BufferSSAO
{
public:
	BufferSSAO() = default;
	~BufferSSAO();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();

	unsigned int gSSAO, Depth;

private:
	const static int attachments = 1;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0};
	unsigned int ID = 0;
};