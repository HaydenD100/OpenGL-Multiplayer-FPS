#include "Light.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Core/Scene/SceneManager.h"
Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 colour, float cutoff, float outercutoff, float linear, float quadratic) {
	this->lighttype = Spotlight;
	this->position = position;
	this->direction = direction;
	this->cutoff = cutoff;
	this->colour = colour;
	this->linear = linear;
	this->quadratic = quadratic;
	this->outercutoff = outercutoff;

	this->lightMax = std::fmaxf(std::fmaxf(colour.r, colour.g), colour.b);
	this->radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (1 - (256.0 / 5.0) * lightMax))) / (2 * quadratic);

	SetUpShadows();
	GenerateShadows();
}


Light::Light(glm::vec3 position, glm::vec3 colour, float linear, float quadratic) {
	this->lighttype = PointLight;
	this->position = position;
	this->colour = colour;
	this->linear = linear;
	this->quadratic = quadratic;
	this->cutoff = 0;
	this->outercutoff = 0;
	this->direction = glm::vec3(0);


	this->lightMax = std::fmaxf(std::fmaxf(colour.r, colour.g), colour.b);
	this->radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (1 - (256.0 / 5.0) * lightMax))) / (2 * quadratic);

	SetUpShadows();
	GenerateShadows();
}

void Light::SetUpShadows() {
	glGenTextures(1, &depthCubemap);
	glGenFramebuffers(1, &depthMapFBO);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	near = 1.0f;
	far = 25.0f;
	shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

}


void Light::GenerateShadows() {
	Renderer::UseProgram(Renderer::GetProgramID("shadow"));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < 6; ++i) {
		std::string pos = "shadowMatrices[" + std::to_string(i) + "]";
		glUniformMatrix4fv(glGetUniformLocation(Renderer::GetCurrentProgramID(), pos.c_str()), 1, GL_FALSE, &shadowTransforms[i][0][0]);
	}
	glUniform1f(glGetUniformLocation(Renderer::GetCurrentProgramID(), "far_plane"), far);
	Renderer::setVec3(glGetUniformLocation(Renderer::GetCurrentProgramID(), "lightPos"), position);

	SceneManager::GetCurrentScene()->RenderObjects(Renderer::GetCurrentProgramID());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << "Generated shadows \n";
}

