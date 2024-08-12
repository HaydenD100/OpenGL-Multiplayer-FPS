#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Engine/Loaders/stb_image.h"

class Texture
{
public:
    //static int CurrentTextureNumber;

    Texture(const char* name, const char* path);
    Texture(const char* name, const char* path, const char* normalPath);
    Texture(const char* name, const char* path, const char* normalPath, const char* specularPath);


    const char* GetName();
    int GetTextureNumber();
    int GetTextureNormalNumber();
    int GetTextureSpecularNumber();


    GLuint GetTexture();
    GLuint GetTextureNormal();
    GLuint GetTextureSpecular();

private:
    const char* name;
    int textureNumber;
    int textureNormalNumber;
    int textureSpecularNumber;

    GLuint texture;
    GLuint textureNormal;
    GLuint textureSpecular;

};