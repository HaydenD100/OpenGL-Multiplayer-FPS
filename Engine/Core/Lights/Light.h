#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum LightType
{
	PointLight,
	Spotlight,
	DirectionalLight,
};


struct Light
{
	LightType lighttype;
	glm::vec3 position = glm::vec3(0,0,0);
	glm::vec3 direction = glm::vec3(0, 0, 0);
	glm::vec3 colour = glm::vec3(1,1,1);

	float linear;
	float quadratic;
	float lightMax;
	float radius;
	float cutoff;
	float outercutoff;

	//shadowStuff
	GLuint depthCubemap = 0;
	GLuint depthMapFBO = 0 ;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	std::vector<glm::mat4> shadowTransforms;
	float aspect;
	float near;
	float far;
	glm::mat4 shadowProj;


	Light(glm::vec3 position, glm::vec3 direction, glm::vec3 colour, float cutoff, float outercutoff, float linear, float quadratic);
	Light(glm::vec3 position, glm::vec3 colour, float linear, float quadratic);

	void SetUpShadows();
	void GenerateShadows();
};



/*
For Attenuation, These values are good starting points for most lights. More info at https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation

Distance	Constant	Linear	Quadratic
7				1.0		0.7		1.8
13				1.0		0.35	0.44
20				1.0		0.22	0.20
32				1.0		0.14	0.07
50				1.0		0.09	0.032
65				1.0		0.07	0.017
100				1.0		0.045	0.0075
160				1.0		0.027	0.0028
200				1.0		0.022	0.0019
325				1.0		0.014	0.0007
600				1.0		0.007	0.0002
3250			1.0		0.0014	0.000007
*/