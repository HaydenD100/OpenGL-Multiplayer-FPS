#include "Light.h"
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
}
