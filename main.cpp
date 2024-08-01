#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Engine.h"
#include "Engine/Backend.h"

int main(int argc, char* argv[]) {
	Backend::init();
	Engine::Run();
	return 0;
}
