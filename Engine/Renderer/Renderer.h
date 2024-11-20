#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Loaders/ShaderLoader.hpp"
#include "Loaders/Loader.hpp"
#include "Engine/Core/UI/Text2D.h" 
#include "Loaders/stb_image.h"
#include "Engine/Core/Lights/Light.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Animation/SkinnedAnimatior.h"
#include "Engine/Renderer/Shader.h"



class SkyBox
{
public:
    SkyBox();
    SkyBox(std::vector<std::string> faces);
    unsigned int GetTextureID();
    unsigned int GetSkyBoxVAO();
private:
    unsigned int textureID;
    unsigned int skyboxVAO, skyboxVBO;
    static float skyboxVertices[108];
};

namespace Renderer
{
    extern Shader s_lighting;
    extern Shader s_skybox;
    extern Shader s_geomerty;
    extern Shader s_ssao;
    extern Shader s_screen;
    extern Shader s_transparent;
    extern Shader s_shadow;
    extern Shader s_decal;
    int init();
    void LoadAllShaders();


    void ClearScreen();
    void SwapBuffers(GLFWwindow* window);

    GLuint GetCurrentProgramID();

    // Text Rendering
    void RenderText(const char* text, int x, int y, int size);

    void RendererSkyBox(glm::mat4 view, glm::mat4 projection, SkyBox skybox);

    void RenderAllObjects(Shader& shader);



    // Shader
    void SetLights(std::vector<Light> lights);
    void RenderScene();

}
