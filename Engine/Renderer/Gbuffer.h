#pragma once
#include "Engine/Core/Common/RenderCommon.h"


class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	void Bind();
	void Destroy();
	void Configure();
	unsigned int GetID();
	
	unsigned int gAlbedo, gNormal, gTrueNormal, gPosition, gRMA, gEmission, gtransparent, Depth, gFur;

private:
	const static int attachments = 8;
	GLenum DrawBuffers[attachments] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
	unsigned int ID = 0;
};