#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 colour, float linear, float quadratic) {
	this->position = position;
	this->colour = colour;
	this->linear = linear;
	this->quadratic = quadratic;


	this->lightMax = std::fmaxf(std::fmaxf(colour.r, colour.g), colour.b);
	this->radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (1 - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
}
