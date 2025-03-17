#pragma once
#include "Engine/Core/Common/RenderCommon.h"

#include <iostream>
#include <vector>
#include "Engine/Loaders/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Texture
{
public:
    //static int CurrentTextureNumber;
    Texture() = default;
    Texture(aiTexture* aitexture, std::string name);
    Texture(const char* name, const char* path, float roughness, float metalic);
    Texture(std::string name);

    Texture(const char* name, const char* path, const char* normalPath, float roughness, float metalic);
    Texture(const char* name, const char* path, const char* normalPath, const char* roughnessPath, const char* metalicPath);
    Texture(aiMaterial* material, const aiScene* scene);

    std::string GetName();
    GLuint GetTexture();
    GLuint GetTextureNormal();
    GLuint GetTextureRoughness();
    GLuint GetTextureMetalic();
    float GetRoughness();
    float GetMetalic();
    bool IsEmissive();
    void SetEmissive(bool state);




private:
    std::string name;

    GLuint texture = NULL;
    GLuint textureNormal = NULL;
    GLuint textureRoughness = NULL;
    GLuint textureMetalic = NULL;

    float roughness = -1;
    float metalic = -1;
    bool IsEmisive = false;

    GLuint AssimpTextureToOpengl(const aiTexture* embeddedTexture);


};