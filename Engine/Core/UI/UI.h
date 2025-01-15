#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Core/Input.h"
#include "Engine/Renderer/Renderer.h"


enum UIState
{
	NONE,
	HOVER,
	CLICKED
};

class UIElement {
public:
	UIElement(glm::vec3 position, int width, int height) {
		this->position = position;
		this->width = width;
		this->height = height;
	}

	UIState State() {
		glm::vec2 mousePos(Input::GetMouseX(), Input::GetMouseY());
		int L = position.x - width / 2;
		int R = position.x + width / 2;
		int T = position.y - height / 2;
		int B = position.y + height / 2;
		if (mousePos.x > L && mousePos.x < R && mousePos.y > T && mousePos.y < B && enabled) {
			if (Input::LeftMouseDown())
				return CLICKED;
			return HOVER;
		}
		return NONE;
	}	

	void Enable() {
		enabled = 1;
	}
	void Disable() {
		enabled = 0;
	}
	virtual void Render();

private:
	int enabled = 1;
	glm::ivec2 position;
	int width, height;
};

class Text : public UIElement {
public:
	Text(std::string text, glm::vec3 position, int textSize) : UIElement{position, 0 ,textSize} {
		this->text = text;
		this->position = position;
		this->textSize = textSize;
	}

	void Render() override {
		unsigned int length = strlen(text.c_str());
		glEnable(GL_BLEND);

		// Fill buffers
		std::vector<glm::vec2> vertices;
		std::vector<glm::vec2> UVs;

		for (unsigned int i = 0; i < length; i++) {
			glm::vec2 vertex_up_left = glm::vec2(position.x + i * textSize, position.y + textSize);
			glm::vec2 vertex_up_right = glm::vec2(position.x + i * textSize + textSize, position.y + textSize);
			glm::vec2 vertex_down_right = glm::vec2(position.x + i * textSize + textSize, position.y);
			glm::vec2 vertex_down_left = glm::vec2(position.x + i * textSize, position.y);

			vertices.push_back(vertex_up_left);
			vertices.push_back(vertex_down_left);
			vertices.push_back(vertex_up_right);

			vertices.push_back(vertex_down_right);
			vertices.push_back(vertex_up_right);
			vertices.push_back(vertex_down_left);

			char character = text[i];
			float uv_x = (character % 16) / 16.0f;
			float uv_y = (character / 16) / 16.0f;

			glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
			glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
			glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
			glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
			UVs.push_back(uv_up_left);
			UVs.push_back(uv_down_left);
			UVs.push_back(uv_up_right);

			UVs.push_back(uv_down_right);
			UVs.push_back(uv_up_right);
			UVs.push_back(uv_down_left);
		}

		glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
		glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

		// Bind shader
		Renderer::s_textShader.Use();

		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(glGetUniformLocation(Renderer::s_textShader.GetShaderID(), "textShader"), 0);

		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw call
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisable(GL_BLEND);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void SetTextureID(unsigned int id, unsigned int Text2DVertexBufferID, unsigned int Text2DUVBufferID) {
		TextureID = id;
		this->Text2DUVBufferID = Text2DUVBufferID;
		this->Text2DVertexBufferID = Text2DVertexBufferID;
	}

private:
	unsigned int Text2DVertexBufferID = 0;
	unsigned int Text2DUVBufferID = 0;
	unsigned int TextureID = 0;

	std::string text = "";
	glm::vec2 position = glm::vec3(0);
	int textSize = 0;
};