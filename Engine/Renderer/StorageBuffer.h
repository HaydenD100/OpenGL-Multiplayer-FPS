#pragma once
#include "Engine/Core/Common/RenderCommon.h"


struct StorageBuffer {
	void Configure(int size);
	void Bind(int index);
	void Bind();

	GLuint GetID();

private:
	GLuint ID;

};