#include "Probe.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Scene/SceneManager.h"
#include "Engine/Renderer/Raycaster.h"


void ProbeGrid::ShowProbes() {

	Renderer::s_probeRender.Use();
	Renderer::s_probeRender.SetMat4("P", Camera::getProjectionMatrix());
	Renderer::s_probeRender.SetMat4("V", Camera::getViewMatrix());

	Renderer::SHBuffer.Bind(7);
	Renderer::probeTexture.Bind(6);

	for (int i = 0; i < probes.size(); i++) {

		Renderer::s_probeRender.SetInt("probeID", probes[i].ProbeID());


		Renderer::s_probeRender.SetMat4("M", probes[i].GetTransform().to_mat4());
		AssetManager::GetModel("probe")->RenderModel(Renderer::s_probeRender.GetShaderID());
	}
}
void ProbeGrid::FillBuffer() {
	for (auto probe : probes) {
		probe.CreateBindless();
		handels.push_back(probe.h_gAlbedo);
		handels.push_back(probe.h_gNormal);
		handels.push_back(probe.h_gPosition);
	}
	glCreateBuffers(1, &b_handles);

	glNamedBufferStorage(
		b_handles,
		sizeof(GLuint64) * handels.size(),
		(const void*)handels.data(),
		GL_DYNAMIC_STORAGE_BIT
	);
}



void ProbeGrid::Bake(std::vector<Light> lights) {
	std::cout << "Starting Probe Baking \n";
	float start = glfwGetTime();
	glViewport(0, 0, PROBESIZE, PROBESIZE);

	glClearColor(0, 0, 0, 1);

	Renderer::s_probeDeffered.Use();
	Renderer::s_probeDeffered.SetVec3("gridWorldPos", postion);
	Renderer::s_probeDeffered.SetVec3("volume", volume);
	Renderer::s_probeDeffered.SetVec3("spacing", spacing);

	Renderer::probeTexture.ImageBind(6);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < probes.size(); i++) {
		probes[i].Bake();
	}
	glEnable(GL_CULL_FACE);


	FillBuffer();

	ReLight(probes.size());

	glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
	glClearColor(0, 0, 0, 1);
	std::cout << "Done Baking " << glfwGetTime() - start <<  "s \n";
}

void ProbeGrid::ReLight(int probeRelightCount) {

	Scene* scene = SceneManager::GetCurrentScene();

	Renderer::cs_probeIrradiance.Use();
	Renderer::cs_probeIrradiance.SetVec3("gridWorldPos", postion);
	Renderer::cs_probeIrradiance.SetVec3("volume", volume);
	Renderer::cs_probeIrradiance.SetVec3("spacing", spacing);
	Renderer::cs_probeIrradiance.SetInt("start_index", updatedIndex);
	Renderer::cs_probeIrradiance.SetVec3("Sky_Color", SceneManager::GetCurrentScene()->GetEnviromentLighting().indirectLight);

	updatedIndex += probeRelightCount;
	if (updatedIndex > probes.size()) {
		probeRelightCount = updatedIndex - probes.size();
		updatedIndex = 0;
	}


	for (int i = 0; i < scene->GetLightsSize(); i++) {
		Renderer::cs_probeIrradiance.SetVec3("lights[" + std::to_string(i) + "].position", scene->GetLight(i)->position);
		Renderer::cs_probeIrradiance.SetVec3("lights[" + std::to_string(i) + "].color", scene->GetLight(i)->colour);
		Renderer::cs_probeIrradiance.SetFloat("lights[" + std::to_string(i) + "].strength", scene->GetLight(i)->strength);
		Renderer::cs_probeIrradiance.SetFloat("lights[" + std::to_string(i) + "].radius", scene->GetLight(i)->radius);

		glActiveTexture(GL_TEXTURE5 + i); // Activate texture unit i
		glBindTexture(GL_TEXTURE_CUBE_MAP, scene->GetLight(i)->depthCubemap); // Bind the depth cubemap to the texture unit
	}

	Renderer::probeTexture.ImageBind(6);
	Renderer::SHBuffer.Bind(7);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, b_handles);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, b_probePosition);

	GLuint num_items = probeRelightCount;
	glDispatchCompute(num_items, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void Probe::CreateBindless() {
	h_gAlbedo = glGetTextureHandleARB(probeAlbedo);
	glMakeTextureHandleResidentARB(h_gAlbedo);
	h_gNormal = glGetTextureHandleARB(probeNormal);
	glMakeTextureHandleResidentARB(h_gNormal);
	h_gPosition = glGetTextureHandleARB(probePosition);
	glMakeTextureHandleResidentARB(h_gPosition);


	//glDeleteTextures(1, &probeAlbedo);
	//glDeleteTextures(1, &probeNormal);
	//glDeleteTextures(1, &probePosition);
	glDeleteFramebuffers(1, &gbufferFBO);

}


Probe::Probe(glm::vec3 postion) {
	transform.position = postion;

	probeID = probeCount;
	probeCount++;

	// Create the cubemap texture for the probe
	

	glGenTextures(1, &probeAlbedo);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeAlbedo);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, PROBESIZE, PROBESIZE, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &probeNormal);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeNormal);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &probePosition);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probePosition);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Create and configure the depth texture

	glGenTextures(1, &Depth);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Depth);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, PROBESIZE, PROBESIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// Configure framebuffer for probe
	glGenFramebuffers(1, &gbufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);
	// Attach depth texture
	// Attach cubemap face for color output
	for (int face = 0; face < 6; ++face) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probePosition, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeNormal, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeAlbedo, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, Depth, 0);
	}

	// Set draw buffers (single output for simplicity here)
	GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, DrawBuffers);

	// Check framebuffer completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete: " << status << std::endl;
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 25.0f);

	//const int rays = 100;
	//for (int i = 0; i < rays; i++ ) {
		//Raycaster::queueRay(postion, glm::vec3(0,0,0), 25, glm::vec4(1, probeID, 0, 0));
	//}
}

void Probe::Irradiance() {
	/*
	glBindFramebuffer(GL_FRAMEBUFFER, this->probeFBO);
	glBindTextureUnit(0, probeAlbedo);
	glBindTextureUnit(1, probeNormal);
	glBindTextureUnit(2, probePosition);
	glBindTextureUnit(3, GetDepthCubeMap());

	Renderer::s_probeirradiance.SetMat4("P", captureProjection);
	Renderer::s_probeirradiance.SetInt("probeID", probeID);
	Renderer::s_probeirradiance.SetVec3("probe_world_Pos", transform.position);

	Renderer::probeTexture.ImageBind(6);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeIrradianceCubemap, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::s_probeirradiance.SetMat4("V", captureViews[i]);
		Renderer::RenderCube();
	}
	*/

}

void Probe::ReLight() {
	/*
	//glBindFramebuffer(GL_FRAMEBUFFER, this->probeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->probeFBO);

	glBindTextureUnit(0, probeAlbedo);
	glBindTextureUnit(1, probeNormal);
	glBindTextureUnit(2, probePosition);
	Renderer::s_probe.SetMat4("P", captureProjection);
	Renderer::s_probe.SetVec3("viewPos", transform.position);

	captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	for (unsigned int i = 0; i < 6; ++i)
	{
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeLighting, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_depth, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeLighting, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::s_probe.SetMat4("V", captureViews[i]);
		//Renderer::s_probe.SetMat4("M", captureViews[i]);
		Renderer::RenderCube();
	}
	*/
}

void Probe::Bake() {
	glViewport(0, 0, PROBESIZE, PROBESIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->gbufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	captureViews[0] = glm::lookAt(transform.position, transform.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(transform.position, transform.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	Renderer::s_probeDeffered.SetMat4("P", captureProjection);
	Renderer::s_probeDeffered.SetVec3("viewPos", this->transform.position);
	Renderer::s_probeDeffered.SetBool("animated", false);
	Renderer::s_probeDeffered.SetVec3("position", transform.position);
	Renderer::s_probeDeffered.SetInt("probeID", probeID);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probePosition, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeNormal, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeAlbedo, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Depth, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Renderer::s_probeDeffered.SetMat4("V", captureViews[i]);

		for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
			GameObject* gameobjectRender = AssetManager::GetGameObject(i);

			if (gameobjectRender->GetShaderType() != "Default" || !gameobjectRender->ShouldRender() || !gameobjectRender->IncludedInGI()) {
				continue;
			}

			glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
			glm::mat4 modelViewMatrix = captureViews[i] * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			Renderer::s_probeDeffered.SetMat3("normalMatrix3", normalMatrix);
			Renderer::s_probeDeffered.SetMat4("M", ModelMatrix);
			gameobjectRender->RenderObject(Renderer::s_probeDeffered.GetShaderID());
		}		
	}

}

Transform Probe::GetTransform() {
	return transform;
}

GLuint Probe::GetCubeAlbedo() {
	return probeAlbedo;
}
GLuint Probe::GetCubeNormal() {
	return probeNormal;
}
GLuint Probe::GetCubePosition() {
	return probePosition;
}

unsigned int Probe::ProbeID() {
	return probeID;
}

unsigned int Probe::probeCount = 0;


