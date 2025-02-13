#pragma once
#include "Engine/Core/Common/RenderCommon.h"

namespace Text2D
{
	void initText2D(const char* texturePath);
	void printText2D(const char* text, int x, int y, int size);
	void cleanupText2D();
	GLuint GetProgramID();
}
