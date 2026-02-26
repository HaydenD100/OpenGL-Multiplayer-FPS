#pragma once
#include "Engine/Core/Common/RenderCommon.h"


class BufferFur
{
public:
	BufferFur() = default;
	~BufferFur();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();

	unsigned int gLighting, Depth;

private:
	const static int attachments = 3;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0};
	unsigned int ID = 0;
};