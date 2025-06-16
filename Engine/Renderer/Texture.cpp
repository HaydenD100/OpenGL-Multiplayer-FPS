#include "Texture.h"


Texture::~Texture() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        std::cout << "Deleted texture: " << texture << "\n";
    }
    if (textureNormal != 0)
        glDeleteTextures(1, &textureNormal);
    if (textureMetalic != 0)
        glDeleteTextures(1, &textureMetalic);
    if (textureRoughness != 0)
        glDeleteTextures(1, &textureRoughness);
}
Texture::Texture(Texture&& other) noexcept {
    texture = other.texture;
    textureNormal = other.textureNormal;
    textureMetalic = other.textureMetalic;
    textureRoughness = other.textureRoughness;
    name = std::move(other.name);

    other.texture = 0;
    other.textureNormal = 0;
    other.textureMetalic = 0;
    other.textureRoughness = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        // Clean up existing
        if (texture) glDeleteTextures(1, &texture);
        if (textureNormal) glDeleteTextures(1, &textureNormal);
        if (textureMetalic) glDeleteTextures(1, &textureMetalic);
        if (textureRoughness) glDeleteTextures(1, &textureRoughness);

        // Move in
        texture = other.texture;
        textureNormal = other.textureNormal;
        textureMetalic = other.textureMetalic;
        textureRoughness = other.textureRoughness;
        name = std::move(other.name);

        other.texture = 0;
        other.textureNormal = 0;
        other.textureMetalic = 0;
        other.textureRoughness = 0;
    }
    return *this;
}
Texture::Texture(aiTexture* aitexture, std::string name) {
    this->name = name;
    this->roughness = 0.5;
    this->metalic = 0;


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    std::cout << "Loading Embbeded Texture \n";

    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, aitexture->mWidth, aitexture->mHeight,
        0, format, GL_UNSIGNED_BYTE, aitexture->pcData);
}

Texture::Texture(std::vector<GLfloat> data, int width, int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data.data()) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
}

Texture::Texture(const char* name, const char* path, float Roughness, float Metalic) {
    this->name = name;
    this->roughness = Roughness;
    this->metalic = Metalic;

    std::cout << "Loading Texture " << path << std::endl;
    CreateTexture(&texture, path, GL_RGBA, GL_RGBA , 4);

}
Texture::Texture(std::string name) {
    this->name = name;

    std::string path = "Assets/Textures/" + name + ".png";
    CreateTexture(&texture, path.c_str(), GL_RGBA, GL_RGBA, STBI_rgb_alpha);
    path = "Assets/Normals/" + name + "_normal.png";
    CreateTexture(&textureNormal, path.c_str(), GL_RGB, GL_RGB, STBI_rgb);
    path = "Assets/Roughness/" + name + "_roughness.png";
    CreateTexture(&textureRoughness, path.c_str(), GL_RED, GL_RED, 1);
    path = "Assets/Metalic/" + name + "_metalic.png";
    CreateTexture(&textureMetalic, path.c_str(), GL_RED, GL_RED, 1);
}


Texture::Texture(const char* name, const char* path, const char* normalPath, float Roughness, float Metalic) {
    this->name = name;
    this->roughness = Roughness;
    this->metalic = Metalic;

    std::cout << "Loading Texture " << path << std::endl;
    CreateTexture(&texture, path, GL_RGBA, GL_RGBA, STBI_rgb_alpha);
    CreateTexture(&textureNormal, normalPath, GL_RGB, GL_RGB, 3);
}

Texture::Texture(const char* name, const char* path, const char* normalPath, std::string roughnessPath, std::string metalicPath) {
    this->name = name;

    std::cout << "Loading Texture " << path << std::endl;

    CreateTexture(&texture, path, GL_RGBA, GL_RGBA, STBI_rgb_alpha);
    CreateTexture(&textureNormal, normalPath, GL_RGB, GL_RGB, STBI_rgb);
    CreateTexture(&textureRoughness, roughnessPath.c_str(), GL_RED, GL_RED, 1);
    CreateTexture(&textureMetalic, metalicPath.c_str(), GL_RED, GL_RED, 1);
}

Texture::Texture(aiMaterial* material, const aiScene* scene) {

    aiString materialName;//The name of the material found in mesh file
    aiReturn ret;//Code which says whether loading something has been successful of not
    aiString texturePathDiffuse;
    aiString texturePathNormal;
    aiString texturePathMetalic;
    aiString texturePathRoughness;

    material->Get(AI_MATKEY_NAME, materialName);//Get the material name (pass by reference)

    name = materialName.C_Str();

    material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePathDiffuse);

    const aiTexture* textureInfo = scene->GetEmbeddedTexture(texturePathDiffuse.C_Str());
    if (textureInfo) {
        texture = AssimpTextureToOpengl(textureInfo);
        textureInfo = nullptr;
    }

    material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), texturePathNormal);
    textureInfo = scene->GetEmbeddedTexture(texturePathNormal.C_Str());
    if (textureInfo) {
        textureNormal = AssimpTextureToOpengl(textureInfo);
        textureInfo = nullptr;
    }

    material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), texturePathRoughness);
    textureInfo = scene->GetEmbeddedTexture(texturePathRoughness.C_Str());
    if (textureInfo) {
        textureRoughness = AssimpTextureToOpengl(textureInfo);
        textureInfo = nullptr;
    }

    material->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), texturePathMetalic);
    textureInfo = scene->GetEmbeddedTexture(texturePathMetalic.C_Str());
    if (textureInfo) {
        textureMetalic = AssimpTextureToOpengl(textureInfo);
        textureInfo = nullptr;
    }


    std::cout << "Material Name Texture: " << materialName.C_Str() << "\n";
}

GLuint Texture::AssimpTextureToOpengl(const aiTexture* embeddedTexture) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (embeddedTexture->mHeight == 0) {
        // Compressed texture (e.g., PNG or JPEG)
        int width, height, channels;

        // Decode compressed data using stb_image
        unsigned char* data = stbi_load_from_memory(
            reinterpret_cast<unsigned char*>(embeddedTexture->pcData),
            embeddedTexture->mWidth,
            &width,
            &height,
            &channels,
            4 // Force 4 channels (RGBA)
        );

        if (data) {
            // Upload texture data to OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Free the image data
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load embedded texture!" << std::endl;
        }
    }
    else {
        // Uncompressed texture (raw data in BGRA format)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, embeddedTexture->mWidth, embeddedTexture->mHeight, 0,
            GL_BGRA, GL_UNSIGNED_BYTE, embeddedTexture->pcData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

    return textureID;
}
void Texture::CreateTexture(GLuint* texture, const char* path, GLenum internalFormat, GLenum format, int stbi_load_format) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load and generate the texture
    int width, height;
    unsigned char* data = stbi_load(path, &width, &height, 0, stbi_load_format);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
}

void Texture::CreateTexture(GLuint* texture, std::vector<GLfloat> data, int width, int height, GLenum internalFormat, GLenum format) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data.data()) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
}



std::string Texture::GetName() {
    return name;
}
GLuint Texture::GetTextureRoughness() {
    return textureRoughness;
}

GLuint Texture::GetTextureMetalic() {
    return textureMetalic;
}
GLuint Texture::GetTextureNormal() {
    return textureNormal;
}
GLuint Texture::GetTexture() {
    return texture;
}
float Texture::GetRoughness() {
    return roughness;
}
float Texture::GetMetalic() {
    return metalic;
}
bool Texture::IsEmissive() {
    return IsEmisive;
}
void Texture::SetEmissive(bool state) {
    IsEmisive = state;
}


