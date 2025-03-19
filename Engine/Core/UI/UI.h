#pragma once
#include "Engine/Core/Common/RenderCommon.h"
#include "Engine/Core/Input.h"
#include "Engine/Renderer/Renderer.h"

namespace ImGuiUI {
	void Init();
	void CleanUpUI();
	void NewFrame();
	void Update();
	void DrawUI();
	void Text(std::string text, glm::vec2 position, float size);
}