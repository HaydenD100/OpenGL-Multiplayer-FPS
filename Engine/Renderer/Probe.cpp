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

		glBindTextureUnit(0, probes[i].GetCubeMap());
		glBindTextureUnit(1, probes[i].GetIrradianceCubeMap());
		Renderer::s_probeRender.SetInt("probeID", probes[i].ProbeID());


		Renderer::s_probeRender.SetMat4("M", probes[i].GetTransform().to_mat4());
		AssetManager::GetModel("probe")->RenderModel(Renderer::s_probeRender.GetShaderID());
	}
}


void ProbeGrid::Bake(std::vector<Light> lights) {
	Renderer::s_probe.Use();

	// Upload lights data to the GPU
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
	Renderer::s_probe.SetVec3Array("lightPos", lightPositions);
	Renderer::s_probe.SetVec3Array("Lightdirection", lightDirection);
	Renderer::s_probe.SetVec3Array("LightColors", lightColors);
	Renderer::s_probe.SetFloatArray("LightLinears", LightLinears);
	Renderer::s_probe.SetFloatArray("LightQuadratics", LightQuadratics);
	Renderer::s_probe.SetFloatArray("LightRadius", LightRadius);
	Renderer::s_probe.SetFloatArray("LightCutOff", LightCutoff);
	Renderer::s_probe.SetFloatArray("LightOuterCutOff", LightOuterCutOff);

	// Upload depth maps (cubemap for shadow mapping)
	for (int i = 0; i < lights.size(); i++) {
		glActiveTexture(GL_TEXTURE4 + i); // Activate texture unit i
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i].depthCubemap); // Bind the depth cubemap to the texture unit
	}

	glDisable(GL_CULL_FACE);
	glViewport(0, 0, PROBESIZE, PROBESIZE);

	//SkyBoxColor
	glClearColor(0, 0, 0, 1);

	for (int i = 0; i < probes.size(); i++) {
		probes[i].Bake();
	}

	

	Renderer::s_probeirradiance.Use();
	Renderer::s_probeirradiance.SetVec3("gridWorldPos", postion);
	Renderer::s_probeirradiance.SetVec3("volume", volume);
	Renderer::s_probeirradiance.SetFloat("spacing", spacing);

	Renderer::probeTexture.ImageBind(6);
	Renderer::SHBuffer.Bind(7);


	for (int i = 0; i < probes.size(); i++) {
		probes[i].Irradiance();
		
		//Raycaster::queueRay(glm::vec3(0, 0, 1), probes[i].GetTransform().position, glm::length(glm::vec3(0, 0, 1)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(0, 1, 0), probes[i].GetTransform().position, glm::length(glm::vec3(0, 1, 0)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(0, 1, 1), probes[i].GetTransform().position, glm::length(glm::vec3(0, 1, 1)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(1, 0, 0), probes[i].GetTransform().position, glm::length(glm::vec3(1, 0, 0)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(1, 0, 1), probes[i].GetTransform().position, glm::length(glm::vec3(1, 0, 1)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(1, 1, 0), probes[i].GetTransform().position, glm::length(glm::vec3(1, 1, 0)), probes[i].ProbeID());
		//Raycaster::queueRay(glm::vec3(1, 1, 1), probes[i].GetTransform().position, glm::length(glm::vec3(1, 1, 1)), probes[i].ProbeID());
		
	}
	glViewport(0, 0, Backend::GetWidth(), Backend::GetHeight());

	glEnable(GL_CULL_FACE);


	glClearColor(0, 0, 0, 1);
}

Probe::Probe(glm::vec3 postion) {
	transform.position = postion;

	probeID = probeCount;
	probeCount++;

	// Create the cubemap texture for the probe
	glGenTextures(1, &probeCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeCubemap);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Create the irradiance cubemap texture
	glGenTextures(1, &probeIrradianceCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, probeIrradianceCubemap);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, PROBESIZE, PROBESIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
	glGenFramebuffers(1, &probeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, probeFBO);

	// Attach depth texture
	// Attach cubemap face for color output
	for (int face = 0; face < 6; ++face) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeCubemap, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, probeIrradianceCubemap, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, Depth, 0);

	}

	// Set draw buffers (single output for simplicity here)
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, DrawBuffers);

	// Check framebuffer completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete: " << status << std::endl;
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 15.0f);


}

void Probe::Irradiance() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->probeFBO);
	glBindTextureUnit(0, GetCubeMap());
	glBindTextureUnit(1, GetDepthCubeMap());


	captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	Renderer::s_probeirradiance.SetMat4("P", captureProjection);
	Renderer::s_probeirradiance.SetInt("probeID", probeID);
	Renderer::s_probeirradiance.SetVec3("probe_world_Pos", transform.position);



	Renderer::probeTexture.ImageBind(6);


	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeIrradianceCubemap, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Renderer::s_probeirradiance.SetMat4("V", captureViews[i]);
		AssetManager::GetModel("cube")->RenderModel(Renderer::s_probeirradiance.GetShaderID());
	}

}


void Probe::Bake() {

	captureViews[0] = glm::lookAt(transform.position, transform.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(transform.position, transform.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(transform.position, transform.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glViewport(0, 0, PROBESIZE, PROBESIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->probeFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Renderer::s_probe.SetMat4("P", captureProjection);
	Renderer::s_probe.SetVec3("viewPos", this->transform.position);
	Renderer::s_probe.SetBool("animated", false);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probeCubemap, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, Depth, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Renderer::s_probe.SetMat4("V", captureViews[i]);

		for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
			GameObject* gameobjectRender = AssetManager::GetGameObject(i);

			if (!gameobjectRender->ShouldRender())
				continue;
			if (gameobjectRender->GetShaderType() != "Default") {
				continue;
			}

			glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
			glm::mat4 modelViewMatrix = captureViews[i] * ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

			Renderer::s_probe.SetMat3("normalMatrix3", normalMatrix);
			Renderer::s_probe.SetMat4("M", ModelMatrix);
			gameobjectRender->RenderObject(Renderer::s_probe.GetShaderID());
		}
		std::vector<Light> lights = SceneManager::GetCurrentScene()->getLights();
		for (int i = 0; i < lights.size(); i++) {
			Renderer::s_probe.SetVec3("color", lights[i].colour / 8.0f);
			glm::mat4 positionMatrix = glm::mat4(); // create an identity matrix;
			positionMatrix = glm::translate(positionMatrix, lights[i].position); //position is a vec3
			Renderer::s_probe.SetMat4("M", positionMatrix);
			AssetManager::GetModel("light_cube")->RenderModel(Renderer::s_probe.GetShaderID());
		}
		Renderer::s_probe.SetVec3("color", glm::vec3(0.0));


	}

}

Transform Probe::GetTransform() {
	return transform;
}
GLuint Probe::GetCubeMap() {
	return probeCubemap;
}
GLuint Probe::GetIrradianceCubeMap() {
	return probeIrradianceCubemap;
}

unsigned int Probe::ProbeID() {
	return probeID;
}
GLuint Probe::GetDepthCubeMap() {
	return Depth;
}



unsigned int Probe::probeCount = 0;


