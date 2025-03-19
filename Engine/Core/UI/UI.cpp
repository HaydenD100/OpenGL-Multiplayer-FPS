#include "UI.h"
#include "Engine/Backend.h"


namespace ImGuiUI {	void Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange; // Allow ImGui to manage the cursor
		
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Block ImGui from changing the cursor
		glfwSetInputMode(Backend::GetWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Force-hide v

		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(Backend::GetWindowPointer(), true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}
	void CleanUpUI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void Update() {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Backend::GetWidth(), (float)Backend::GetHeight());
	}
	void DrawUI() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void Text(std::string text, glm::vec2 position, float size) {
		ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
		ImGui::Begin(
			text.c_str(),
			nullptr,
			ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoInputs       // Make the overlay non-interactive
			| ImGuiWindowFlags_NoBackground
		);
		ImGui::Text(text.c_str());
		ImGui::End();
	}


}