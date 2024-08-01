#include "Window.h"

Window::Window(std::string name, const char* frame_path, Texture* frame_texture, const char* window_path, Texture* window_texture, glm::vec3 position, glm::vec3 rotation) {
	this->name = name;
	window = GameObject(name, window_path, window_texture, position, false,0,Box);
	window.setRotation(rotation);
	//You want the frame to be rendered before the windows are
	AssetManager::AddGameObject(name + "_frame", frame_path, frame_texture, position, false,0,Box);
	AssetManager::GetGameObject(name + "_frame")->setRotation(rotation);
}

void Window::Render(GLuint programID, glm::mat4 View, glm::mat4 projection) {
	if (window.ShouldRender())
	{ 
		glm::mat4 ModelMatrix = window.GetModelMatrix();
		glm::mat4 MVP = projection * View * ModelMatrix;
		glm::mat3 ModelView3x3Matrix = glm::mat3(View * ModelMatrix); // Take the upper-left part of ModelViewMatrix
		Renderer::SetTextureShader(MVP, ModelMatrix, View, ModelView3x3Matrix);
		window.RenderObject(programID);
	}
}
