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
    Texture() = default;
    Texture(const char* name, const char* path, float roughness, float metalic);
    Texture(const char* name, const char* path, const char* normalPath, float roughness, float metalic);
    Texture(const char* name, const char* path, const char* normalPath, const char* roughnessPath, const char* metalicPath);

    const char* GetName();
    GLuint GetTexture();
    GLuint GetTextureNormal();
    GLuint GetTextureRoughness();
    GLuint GetTextureMetalic();
    float GetRoughness();
    float GetMetalic();
    bool IsEmissive();
    void SetEmissive(bool state);




private:
    const char* name;

    GLuint texture;
    GLuint textureNormal;
    GLuint textureRoughness;
    GLuint textureMetalic;

    float roughness = -1;
    float metalic = -1;
    bool IsEmisive = false;


};