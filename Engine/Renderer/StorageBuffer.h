#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct StorageBuffer {
	void Configure(int size);
	void Bind(int index);

	GLuint GetID();




private:
	GLuint ID;

};