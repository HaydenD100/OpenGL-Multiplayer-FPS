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

		glBindTextureUnit(0, probes[i].GetCubeLighting());
		glBindTextureUnit(1, probes[i].GetIrradianceCubeMap());
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
	std::cout << "starting baking lighting \n";
	glViewport(0, 0, PROBESIZE, PROBESIZE);

	glClearColor(0, 0, 0, 1);

	Renderer::s_probeDeffered.Use();
	Renderer::s_probeDeffered.SetVec3("gridWorldPos", postion);
	Renderer::s_probeDeffered.SetVec3("volume", volume);
	Renderer::s_probeDeffered.SetFloat("spacing", spacing);

	Renderer::probeTexture.ImageBind(6);

	for (int i = 0; i < probes.size(); i++) {
		probes[i].Bake();
	}

	glDisable(GL_CULL_FACE);

	FillBuffer();

	ReLight(lights, probes.size());
	/*

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightDirection;

	std::vector<glm::vec3> lightColors;
	std::vector<float> LightLinears;
	std::vector<float> LightQuadratics;
	std::vector<float> LightRadius;
	std::vector<float> LightCutoff;
	std::vector<float> LightOuterCutOff;


	for (const auto& light : lights) {
		lightPositions.push_back(light.position);
		lightDirection.push_back(light.direction);
		lightColors.push_back(light.colour);
		LightLinears.push_back(light.linear);
		LightQuadratics.push_back(light.quadratic);
		LightRadius.push_back(light.radius);
		LightCutoff.push_back(light.cutoff);
		LightOuterCutOff.push_back(light.outercutoff);
	}

	Renderer::s_probeirradiance.Use();
	Renderer::s_probeirradiance.SetVec3("gridWorldPos", postion);
	Renderer::s_probeirradiance.SetVec3("volume", volume);
	Renderer::s_probeirradiance.SetFloat("spacing", spacing);

	Renderer::s_probeirradiance.SetVec3Array("lightPos", lightPositions);
	Renderer::s_probeirradiance.SetVec3Array("Lightdirection", lightDirection);
	Renderer::s_probeirradiance.SetVec3Array("LightColors", lightColors);
	Renderer::s_probeirradiance.SetFloatArray("LightLinears", LightLinears);
	Renderer::s_probeirradiance.SetFloatArray("LightQuadratics", LightQuadratics);
	Renderer::s_probeirradiance.SetFloatArray("LightRadius", LightRadius);
	Renderer::s_probeirradiance.SetFloatArray("LightCutOff", LightCutoff);
	Renderer::s_probeirradiance.SetFloatArray("LightOuterCutOff", LightOuterCutOff);

	// Upload depth maps (cubemap for shadow mapping)
	for (int i = 0; i < lights.size() && i < 17; i++) {
		glActiveTexture(GL_TEXTURE5 + i); // Activate texture unit i
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
	}

	Renderer::probeTexture.ImageBind(6);
	Renderer::SHBuffer.Bind(7);


	for (int i = 0; i < probes.size(); i++) {
		probes[i].Irradiance(); 
	}
	*/
	glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
	glEnable(GL_CULL_FACE);
	glClearColor(0, 0, 0, 1);
	std::cout << "Done Baking \n";
}

void ProbeGrid::ReLight(std::vector<Light> lights, int probeRelightCount) {
	//glViewport(0, 0, PROBESIZE, PROBESIZE);
	//glClearColor(0, 0, 0, 1);

	//glDisable(GL_CULL_FACE);

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightDirection;

	std::vector<glm::vec3> lightColors;
	std::vector<float> LightLinears;
	std::vector<float> LightQuadratics;
	std::vector<float> LightRadius;
	std::vector<float> LightCutoff;
	std::vector<float> LightOuterCutOff;


	for (const auto& light : lights) {
		lightPositions.push_back(light.position);
		lightDirection.push_back(light.direction);
		lightColors.push_back(light.colour);
		LightLinears.push_back(light.linear);
		LightQuadratics.push_back(light.quadratic);
		LightRadius.push_back(light.radius);
		LightCutoff.push_back(light.cutoff);
		LightOuterCutOff.push_back(light.outercutoff);
	}
	Renderer::cs_probeIrradiance.Use();

	Renderer::cs_probeIrradiance.Use();
	Renderer::cs_probeIrradiance.SetVec3("gridWorldPos", postion);
	Renderer::cs_probeIrradiance.SetVec3("volume", volume);
	Renderer::cs_probeIrradiance.SetFloat("spacing", spacing);



	Renderer::cs_probeIrradiance.SetInt("start_index", updatedIndex);

	updatedIndex += probeRelightCount;
	if (updatedIndex > probes.size()) {
		probeRelightCount = updatedIndex - probes.size();
		updatedIndex = 0;
	}

	Renderer::cs_probeIrradiance.SetVec3Array("lightPos", lightPositions);
	Renderer::cs_probeIrradiance.SetVec3Array("Lightdirection", lightDirection);
	Renderer::cs_probeIrradiance.SetVec3Array("LightColors", lightColors);
	Renderer::cs_probeIrradiance.SetFloatArray("LightLinears", LightLinears);
	Renderer::cs_probeIrradiance.SetFloatArray("LightQuadratics", LightQuadratics);
	Renderer::cs_probeIrradiance.SetFloatArray("LightRadius", LightRadius);
	Renderer::cs_probeIrradiance.SetFloatArray("LightCutOff", LightCutoff);
	Renderer::cs_probeIrradiance.SetFloatArray("LightOuterCutOff", LightOuterCutOff);

	// Upload depth maps (cubemap for shadow mapping)
	for (int i = 0; i < lights.size() && i < 17; i++) {
		glActiveTexture(GL_TEXTURE5 + i); // Activate texture unit i
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
	}

	Renderer::probeTexture.ImageBind(6);
	Renderer::SHBuffer.Bind(7);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, b_handles);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, b_probePosition);


	glDispatchCompute(probeRelightCount, 1, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	/*

	int temp_index = updatedIndex + probeRelightCount;
	while(updatedIndex < temp_index){
		if (updatedIndex >= probes.size() - 1) {
			updatedIndex = 0;
			break;
		}
		probes[updatedIndex].Irradiance();
		updatedIndex++;
	}
	glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());
	glEnable(GL_CULL_FACE);
	glClearColor(0, 0, 0, 1);
	*/
}

void Probe::CreateBindless() {
	h_gAlbedo = glGetTextureHandleARB(probeAlbedo);
	glMakeTextureHandleResidentARB(h_gAlbedo);
	h_gNormal = glGetTextureHandleARB(probeNormal);
	glMakeTextureHandleResidentARB(h_gNormal);
	h_gPosition = glGetTextureHandleARB(probePosition);
	glMakeTextureHandleResidentARB(h_gPosition);
}


Probe::Probe(glm::vec3 postion) {
	transform.position = postion;

	probeID = probeCount;
	probeCount++;

	// Create the cubemap texture for the probe
	

	glGenTextures(1, &probeAlbedo);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeAlbedo);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_FLOAT, NULL);
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

	glGenTextures(1, &probeLighting);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeLighting);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &probeIrradianceCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeIrradianceCubemap);
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

	
	glGenTextures(1, &m_depth);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth);
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

	
	glGenFramebuffers(1, &probeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, probeFBO);
	// Attach cubemap face for color output
	for (int face = 0; face < 6; ++face) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeLighting, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeIrradianceCubemap, 0);
	}
	GLenum DrawBuffers1[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, DrawBuffers1);

	// Check framebuffer completeness
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete: " << status << std::endl;
	}
	

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 15.0f);

	//const int rays = 100;
	//for (int i = 0; i < rays; i++ ) {
		//Raycaster::queueRay(postion, glm::vec3(0,0,0), 25, glm::vec4(1, probeID, 0, 0));
	//}
}

void Probe::Irradiance() {
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

}

void Probe::ReLight() {
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
GLuint Probe::GetIrradianceCubeMap() {
	return probeIrradianceCubemap;
}
GLuint Probe::GetCubeLighting() {
	return probeLighting;
}

unsigned int Probe::ProbeID() {
	return probeID;
}
GLuint Probe::GetDepthCubeMap() {
	return Depth;
}



unsigned int Probe::probeCount = 0;


