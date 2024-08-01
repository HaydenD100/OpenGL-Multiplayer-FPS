#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 colour, float strength) {
	this->position = position;
	this->colour = colour;
	this->strength = strength;
}
