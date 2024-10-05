#include "Renderer.h"
#include "Scene/SceneManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Core/DecalInstance.h"

#include <random>




SkyBox::SkyBox() = default;

SkyBox::SkyBox(std::vector<std::string> faces) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else {
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

unsigned int SkyBox::GetSkyBoxVAO() {
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

namespace Renderer
{
	std::map<const char*, GLuint> shaderProgramIds;

	GLuint LightID;
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint ModelView3x3MatrixID;
	GLuint gPosition;
	GLuint P;
	GLuint V;

	GLuint ubo;
	//deffered rendering stuff
	GLuint FramebufferName = 0;
	GLuint gPostion;
	GLuint gNormal;
	GLuint gAlbeido;
	GLuint gPBR;




	GLuint depthrenderbuffer;
	GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	GLuint quad_vertexbuffer;
	GLuint depthTexture;

	//ssao
	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;
	unsigned int noiseTexture;
	unsigned int ssaoColorBuffer;
	GLuint ssaoFBO = 0;

	void Renderer::LoadAllShaders() {
		if (shaderProgramIds.size() > 0) {
			for (auto const& [key, val] : shaderProgramIds)
			{
				glDeleteProgram(val);
			}
			shaderProgramIds.clear();
		}
		LoadShader("Assets/Shaders/Lighting/lighting.vert", "Assets/Shaders/Lighting/lighting.frag", "lighting");
		LoadShader("Assets/Shaders/SkyBoxShader.vert", "Assets/Shaders/SkyBoxShader.frag", "skybox");
		LoadShader("Assets/Shaders/Deffered/geomerty.vert", "Assets/Shaders/Deffered/geomerty.frag", "geomerty");
		LoadShader("Assets/Shaders/SSAO/ssao.vert", "Assets/Shaders/SSAO/ssao.frag", "ssao");
		LoadShader("Assets/Shaders/Texture_Render/Texture_Render.vert", "Assets/Shaders/Texture_Render/Texture_Render.frag", "screen");
		LoadShader("Assets/Shaders/Transparent/transparent.vert", "Assets/Shaders/Transparent/transparent.frag", "transparent");
		LoadShader("Assets/Shaders/Shadow/depth.vert", "Assets/Shaders/Shadow/depth.frag", "Assets/Shaders/Shadow/depth.gs", "shadow");
		LoadShader("Assets/Shaders/Decal/decal.vert", "Assets/Shaders/Decal/decal.frag", "decal");


		UseProgram(GetProgramID("lighting"));
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gPostion"), 0);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gNormal"), 1);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gAlbeido"), 2);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gPBR"), 3);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "ssaoTexture"), 4);

		UseProgram(GetProgramID("transparent"));
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gAlbeido"), 0);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gPositionTexture"), 1);

		UseProgram(GetProgramID("geomerty"));
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "DiffuseTextureSampler"), 0);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "NormalTextureSampler"), 1);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "RoughnessTextureSampler"), 2);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "MetalicTextureSampler"), 3);

		MatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "MVP");
		ViewMatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "V");
		ModelMatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "M");
		LightID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "LightPosition_worldspace");
		ModelView3x3MatrixID = glGetUniformLocation(Renderer::GetCurrentProgramID(), "MV3x3");
		P = glGetUniformLocation(Renderer::GetCurrentProgramID(), "P");
		V = glGetUniformLocation(Renderer::GetCurrentProgramID(), "V");

		UseProgram(GetProgramID("ssao"));
		glUniform3fv(glGetUniformLocation(GetCurrentProgramID(), "samples"), (GLsizei)ssaoKernel.size(), glm::value_ptr(ssaoKernel[0]));
		glUniform1f(glGetUniformLocation(GetCurrentProgramID(), "ScreenWidth"), SCREENWIDTH);
		glUniform1f(glGetUniformLocation(GetCurrentProgramID(), "ScreenHeight"), SCREENHEIGHT);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gPostion"), 0);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gNormal"), 1);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "texNoise"), 2);

		UseProgram(GetProgramID("decal"));
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "decalTexture"), 1);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "decalNormal"), 2);
		glUniform1i(glGetUniformLocation(GetCurrentProgramID(), "gDepth"), 3);


		glUniform2f(glGetUniformLocation(GetCurrentProgramID(), "resolution"), SCREENWIDTH, SCREENHEIGHT);


		std::cout << "Done loading shaders \n";
	}

	
	int Renderer::init() {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);  // Cull back-facing triangles
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		//SSAO
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0f;

			// scale samples s.t. they're more aligned to center of kernel
			scale = 0.1f + (scale * scale) * (0.1f - 0.1f);

			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}

		LoadAllShaders();

		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glGenTextures(1, &gAlbeido);
		glBindTexture(GL_TEXTURE_2D, gAlbeido);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glGenTextures(1, &gPBR);
		glBindTexture(GL_TEXTURE_2D, gPBR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
		// Set "renderedTexture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gPosition, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, gNormal, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, gAlbeido, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, gPBR, 0);

		// Set the list of draw buffers.
		glDrawBuffers(4, DrawBuffers); // "1" is the size of DrawBuffer


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "problem \n";

		static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);



		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenFramebuffers(1, &ssaoFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREENWIDTH, SCREENHEIGHT, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
		return 0;
	}

	GLuint Renderer::GetProgramID(const char* name) {
		return shaderProgramIds[name];
	}

	void Renderer::SetTextureShader(glm::mat4 mvp, glm::mat4 model, glm::mat4 view, glm::mat3 ModelView3x3Matrix) {
		setMat4(MatrixID, mvp);
		setMat4(ModelMatrixID, model); 
		setMat4(ViewMatrixID, view);
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
	}

	
	void Renderer::SetLights(std::vector<Light> lights) {
		// Upload lights data to the GPU
		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightDirection;

		std::vector<glm::vec3> lightColors;
		std::vector<float> LightLinears;
		std::vector<float> LightQuadratics;
		std::vector<float> LightRadius;
		std::vector<float> LightCutoff;
		std::vector<float> LightOuterCutOff;
		std::vector<GLuint> depthMap;


		for (const auto& light : lights) {
			lightPositions.push_back(light.position);
			lightDirection.push_back(light.direction);
			lightColors.push_back(light.colour);
			LightLinears.push_back(light.linear);
			LightQuadratics.push_back(light.quadratic);
			LightRadius.push_back(light.radius);
			LightCutoff.push_back(light.cutoff);
			LightOuterCutOff.push_back(light.outercutoff);
			depthMap.push_back(light.depthCubemap);
		}

		

		// Set up uniform arrays in the shader
		GLuint lightPositionsLoc = glGetUniformLocation(GetCurrentProgramID(), "LightPositions_worldspace");
		GLuint lightColorsLoc = glGetUniformLocation(GetCurrentProgramID(), "LightColors");
		GLuint LightLinearsLoc = glGetUniformLocation(GetCurrentProgramID(), "LightLinears");
		GLuint LightQuadraticsLoc = glGetUniformLocation(GetCurrentProgramID(), "LightQuadratics");
		GLuint LightRadiusLoc = glGetUniformLocation(GetCurrentProgramID(), "LightRadius");
		GLuint LightCutoffLoc = glGetUniformLocation(GetCurrentProgramID(), " LightCutOff");
		GLuint LightDirectionLoc = glGetUniformLocation(GetCurrentProgramID(), "Lightdirection");
		GLuint LightOuterCutOffLoc = glGetUniformLocation(GetCurrentProgramID(), "LightOuterCutOff");


		glUniform3fv(lightPositionsLoc, (GLsizei)lights.size(), glm::value_ptr(lightPositions[0]));
		glUniform3fv(LightDirectionLoc, (GLsizei)lights.size(), glm::value_ptr(lightDirection[0]));
		glUniform3fv(lightColorsLoc, (GLsizei)lights.size(), glm::value_ptr(lightColors[0]));
		glUniform1fv(LightLinearsLoc, (GLsizei)lights.size(), &LightLinears[0]);
		glUniform1fv(LightQuadraticsLoc, (GLsizei)lights.size(), &LightQuadratics[0]);
		glUniform1fv(LightRadiusLoc, (GLsizei)lights.size(), &LightRadius[0]);
		glUniform1fv(LightCutoffLoc, (GLsizei)lights.size(), &LightCutoff[0]);
		glUniform1fv(LightOuterCutOffLoc, (GLsizei)lights.size(), &LightOuterCutOff[0]);


		// Upload depth maps (cubemap for shadow mapping)
		for (int i = 0; i < lights.size(); i++) {
			glActiveTexture(GL_TEXTURE5 + i); // Activate texture unit i
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap[i]); // Bind the depth cubemap to the texture unit
			GLuint depthMapLoc = glGetUniformLocation(GetCurrentProgramID(), ("depthMap[" + std::to_string(i) + "]").c_str());
			glUniform1i(depthMapLoc, 5 + i); // Assign the texture unit to the samplerCube array in the shader
		}

	}

	void Renderer::setMat4(GLuint id, glm::mat4& mat4) {
		glUniformMatrix4fv(id, 1, GL_FALSE, &mat4[0][0]);
	}
	
	void Renderer::setVec3(GLuint id, glm::vec3& vec3) {
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
	int Renderer::LoadShader(const char* vertex, const char* fragment, const char* geomtery, const char* name) {
		shaderProgramIds[name] = LoadShaders::LoadShaders(vertex, fragment,geomtery);
		return shaderProgramIds[name];
	}

	void Renderer::RenderText(const char* text,int x, int y, int size) {
		Renderer::UseProgram(Text2D::GetProgramID());
		Text2D::printText2D(text, x, y, size);
	}

	void Renderer::RenderScene() {
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLuint programid = Renderer::GetProgramID("geomerty");
		Renderer::UseProgram(programid);
		glUniformMatrix4fv(P, 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
		glUniformMatrix4fv(V, 1, GL_FALSE, &Camera::getViewMatrix()[0][0]);

		SceneManager::Render(programid);


		//-----------------------------------------Decal---------------------------------------
		glEnable(GL_BLEND);

		programid = Renderer::GetProgramID("decal");
		Renderer::UseProgram(programid);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniformMatrix4fv(glGetUniformLocation(programid, "P"), 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "V"), 1, GL_FALSE, &Camera::getViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "inverseV"), 1, GL_FALSE, &glm::inverse(Camera::getViewMatrix())[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "inverseP"), 1, GL_FALSE, &glm::inverse(Camera::getProjectionMatrix())[0][0]);

		std::vector<DecalInstance>* decals = AssetManager::GetAllDecalInstances();
		for (int i = 0; i < decals->size(); i++) {
			DecalInstance& decal = (*decals)[i];

			// Skip decals with null parents or those outside the camera frustum
			if (decal.CheckParentIsNull() || !decal.GetAABB()->isOnFrustum(Camera::GetFrustum(), decal.getTransform()))
				continue;
			decal.GetDecal()->AddInstace(&decal);
		}

		GLuint sizeLoc = glGetUniformLocation(programid, "size");

		std::vector<Decal>* decalsToBeRendered = AssetManager::GetAllDecals();
		for (int i = 0; i < decalsToBeRendered->size(); i++) {
			Decal& decal = (*decalsToBeRendered)[i];
			glm::vec3 size = decal.GetSize();

			Renderer::setVec3(sizeLoc, size);
			decal.RenderDecal(programid);
		}

		//-----------------------------------------Transaprent stuff---------------------------------------
		programid = Renderer::GetProgramID("transparent");
		Renderer::UseProgram(programid);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gAlbeido);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glUniformMatrix4fv(glGetUniformLocation(programid, "P"), 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "V"), 1, GL_FALSE, &Camera::getViewMatrix()[0][0]);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		std::vector<GameObject*> needRendering = SceneManager::GetCurrentScene()->NeedRenderingObjects();
		glm::vec3 cameraPosition = Camera::GetPosition(); // Camera position

		std::sort(needRendering.begin(), needRendering.end(),
			[&cameraPosition](const GameObject* a, const GameObject* b) {
				float distanceA = glm::length(a->GetPosition() - cameraPosition);
				float distanceB = glm::length(b->GetPosition() - cameraPosition);

				return distanceA > distanceB; // Sort by descending distance (farthest first)
			});

		GLuint modelmatloc = glGetUniformLocation(programid, "M");
		GLuint modelmat3x3loc = glGetUniformLocation(programid, "normalMatrix3");


		std::vector<GameObject*> overlay; 
		for (int i = 0; i < needRendering.size(); i++) {
			if (needRendering[i]->GetShaderType() == "Overlay") {
				overlay.push_back(needRendering[i]);
				continue;
			}

			glm::mat4 ModelMatrix = needRendering[i]->GetModelMatrix();
			glm::mat4 modelViewMatrix = Camera::getViewMatrix() * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
			glUniformMatrix3fv(modelmat3x3loc, 1, GL_FALSE, &normalMatrix[0][0]);
			glUniformMatrix4fv(modelmatloc, 1, GL_FALSE, &ModelMatrix[0][0]);
			needRendering[i]->RenderObject(programid);
		}
		glDisable(GL_BLEND);

		//---------------------------------------------------Overlay-------------------------------------
		programid = Renderer::GetProgramID("geomerty");
		Renderer::UseProgram(programid);

		glClear(GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < overlay.size(); i++) {
			glm::mat4 ModelMatrix = overlay[i]->GetModelMatrix();
			glm::mat4 modelViewMatrix = Camera::getViewMatrix() * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			glUniformMatrix3fv(glGetUniformLocation(programid, "normalMatrix3"), 1, GL_FALSE, &normalMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(programid, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
			overlay[i]->RenderObject(programid);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//---------------------------------------------------SSAO-------------------------------------
		programid = Renderer::GetProgramID("ssao");
		Renderer::UseProgram(programid);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glUniformMatrix4fv(glGetUniformLocation(programid, "projection"), 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
		

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//---------------------------------------------------Lighting-------------------------------------
		programid = GetProgramID("lighting");
		glUseProgram(programid);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbeido);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gPBR);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		
		glUniform3fv(glGetUniformLocation(programid, "viewPos"),1, &Camera::GetPosition()[0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "inverseV"), 1, GL_FALSE, &glm::inverse(Camera::getViewMatrix())[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "V"), 1, GL_FALSE, &Camera::getViewMatrix()[0][0]);

		SetLights(SceneManager::GetCurrentScene()->getLights());
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		glDisable(GL_DEPTH_TEST);
	}


	
	void Renderer::RendererSkyBox(glm::mat4 view, glm::mat4 projection, SkyBox skybox) {
		/*
		glDepthMask(GL_FALSE);
		UseProgram(GetProgramID("skybox"));
		GLuint projectionid = glGetUniformLocation(GetProgramID("skybox"), "projection");
		GLuint viewid = glGetUniformLocation(GetProgramID("skybox"), "view");
		glm::mat4 viewWithoutTranslation = glm::mat4(glm::mat3(view));


		setMat4(viewid, viewWithoutTranslation);
		setMat4(projectionid, projection);
		*/
		//glBindVertexArray(skybox.GetSkyBoxVAO());
		//glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetTextureID());
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDepthMask(GL_TRUE);
	}

	void Renderer::SwapBuffers(GLFWwindow* window) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	GLuint Renderer::GetCurrentProgramID() {
		GLint currentProgramID;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramID);
		return currentProgramID;
	}
}
