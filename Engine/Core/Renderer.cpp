#pragma once
#include "Renderer.h"
#include "Scene/SceneManager.h"

Texture::Texture(const char* name, const char* path) {
	this->name = name;
	int texturenumTemp = CurrentTextureNumber++;
	textureNumber = texturenumTemp - GL_TEXTURE0;

	std::cout << "TextureNumbers loaded at " << textureNumber << std::endl;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height;
	unsigned char* data = stbi_load(path, &width, &height, 0, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	textureNormalNumber = CurrentTextureNumber++ - GL_TEXTURE0;

	std::cout << "Texture Numbers Normal loaded at " << textureNormalNumber << std::endl;

	glGenTextures(1, &textureNormal);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	
	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Load and generate the texture
	int width1, height1;

	unsigned char* data1 = stbi_load("Assets/Normals/no_normal.png", &width1, &height1, 0, STBI_rgb_alpha);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data1);
}
Texture::Texture(const char* name, const char* path, const char* normalPath) {
	this->name = name;
	int texturenumTemp = CurrentTextureNumber++;
	textureNumber = texturenumTemp - GL_TEXTURE0;

	std::cout << "TextureNumbers loaded at " << textureNumber << std::endl;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Load and generate the texture
	int width, height;
	unsigned char* data = stbi_load(path, &width, &height, 0, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	textureNormalNumber = CurrentTextureNumber++ - GL_TEXTURE0;

	std::cout << "Texture Numbers Normal loaded at " << textureNormalNumber << std::endl;

	glGenTextures(1, &textureNormal);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	
	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Load and generate the texture
	int width1, height1;

	unsigned char* data1 = stbi_load(normalPath, &width1, &height1, 0, STBI_rgb_alpha);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data1);
}
int Texture::GetTextureNormalNumber() {
	return textureNormalNumber;
}
GLuint Texture::GetTextureNormal() {
	return textureNormal;
}



const char* Texture::GetName() {
	return name;
}
int Texture::GetTextureNumber() {
	return textureNumber;
}
GLuint Texture::GetTexture() {
	return texture;
}

int Texture::CurrentTextureNumber = GL_TEXTURE1;


SkyBox::SkyBox() {

}


SkyBox::SkyBox(std::vector<std::string> faces) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

unsigned int SkyBox::GetTextureID() {
	return textureID;
}
unsigned int SkyBox::GetSkyBoxVAO(){
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return skyboxVAO;
}
float SkyBox::skyboxVertices[108] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

namespace Renderer {
	std::map<const char*, GLuint> shaderProgramIds;

	GLuint LightID;
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint ModelView3x3MatrixID;
	GLuint gPosition;


	GLuint ubo;




	unsigned int quadVAO;
	unsigned int VBO;

	unsigned int depthMapFBO;
	unsigned int depthMap;





	GLuint Renderer::GetProgramID(const char* name) {
		return shaderProgramIds[name];
	}

	void Renderer::SetTextureShader(glm::mat4 mvp, glm::mat4 model, glm::mat4 view, glm::mat3 ModelView3x3Matrix) {
		setMat4(MatrixID, mvp);
		setMat4(ModelMatrixID, model);
		setMat4(ViewMatrixID, view);
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

	}
	void Renderer::SetLights(std::vector<Light> lights)
	{
		// Upload lights data to the GPU
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;
		std::vector<float> lightPowers;

		for (const auto& light : lights) {
			lightPositions.push_back(light.position);
			lightColors.push_back(light.colour);
			lightPowers.push_back(light.strength);
		}

		// Set up uniform arrays in the shader
		GLuint lightPositionsLoc = glGetUniformLocation(GetProgramID("Texture"), "LightPositions_worldspace");
		GLuint lightColorsLoc = glGetUniformLocation(GetProgramID("Texture"), "LightColors");
		GLuint lightPowersLoc = glGetUniformLocation(GetProgramID("Texture"), "LightPowers");

		glUniform3fv(lightPositionsLoc, lights.size(), glm::value_ptr(lightPositions[0]));
		glUniform3fv(lightColorsLoc, lights.size(), glm::value_ptr(lightColors[0]));
		glUniform1fv(lightPowersLoc, lights.size(), &lightPowers[0]);
		//setVec3(LightID, lights[0].position);

	}

	int Renderer::init(const char* vertex, const char* fragment, const char* name) {
		UseProgram(LoadShader(vertex, fragment, name));
		std::cout << "Loaded texture shader at: " << GetProgramID("Texture") << std::endl;
		

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		MatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "MVP");
		ViewMatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "V");
		ModelMatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "M");
		LightID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "LightPosition_worldspace");
		ModelView3x3MatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "MV3x3");

		// Skybox
		LoadShader("Assets/Shaders/SkyBoxShader.vert", "Assets/Shaders/SkyBoxShader.frag", "skybox");
		std::cout << "Loaded skybox shader at: " << GetProgramID("skybox") << std::endl;
		LoadShader("Assets/Shaders/shaderSprite.vert", "Assets/Shaders/shaderSprite.frag", "sprite");
		std::cout << "Loaded sprite shader at: " << GetProgramID("sprite") << std::endl;

		UseProgram(GetProgramID("sprite"));
		
		// Configure VAO/VBO
		float vertices[] = {
			// pos      // tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		glGenVertexArrays(1, &Renderer::quadVAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindVertexArray(Renderer::quadVAO);

		UseProgram(GetProgramID(name));

		return 0;
	}


	void Renderer::setMat4(GLuint id, glm::mat4& mat4)
	{
		glUniformMatrix4fv(id, 1, GL_FALSE, &mat4[0][0]);
	}
	void Renderer::setVec3(GLuint id, glm::vec3& vec3)
	{
		glUniform3f(id, vec3.x, vec3.y, vec3.z);
	}
	void Renderer::ClearScreen() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::UseProgram(int ProgramID) {
		glUseProgram(ProgramID);
	}
	int Renderer::LoadShader(const char* vertex, const char* fragment, const char* name) {
		shaderProgramIds[name] = LoadShaders::LoadShaders(vertex, fragment); 
		return shaderProgramIds[name];
	} 

	void Renderer::RenderText(const char* text,int x, int y, int size) {
		Renderer::UseProgram(Text2D::GetProgramID());
		Text2D::printText2D(text, x, y, size);
	}

	// TODO: Doesn't work right now still in progress
	void Renderer::DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
		UseProgram(GetProgramID("sprite"));

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribPointer(VBO, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));

		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
		model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

		model = glm::scale(model, glm::vec3(size, 1.0f));

		setVec3(glGetUniformLocation(GetProgramID("sprite"), "spriteColor"),color);
		setMat4(glGetUniformLocation(GetProgramID("sprite"), "model"),model);
		
		glActiveTexture(texture->GetTextureNumber() + GL_TEXTURE0);

		GLuint TextureID = glGetUniformLocation(GetProgramID("sprite"), "image");

		glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
		glUniform1i(TextureID, texture->GetTextureNumber());
		

		glBindVertexArray(Renderer::quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}


	void Renderer::RendererSkyBox(glm::mat4 view, glm::mat4 projection, SkyBox skybox) {
		glDepthMask(GL_FALSE);
		UseProgram(GetProgramID("skybox"));
		GLuint projectionid = glGetUniformLocation(GetProgramID("skybox"), "projection");
		GLuint viewid = glGetUniformLocation(GetProgramID("skybox"), "view");
		glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));

		setMat4(viewid, viewWithoutTranslation);
		setMat4(projectionid, projection);
		glBindVertexArray(skybox.GetSkyBoxVAO());
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetTextureID());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}

	void Renderer::SwapBuffers(GLFWwindow* window) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	GLuint Renderer::GetCurrentProgramID() {
		GLint currentProgramID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramID);
		return currentProgramID;
	}

}

