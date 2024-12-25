#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

class Texture3D {
public:
    Texture3D() = default;
    void Create(int width, int height, int depth);
    void Bind(unsigned int slot);
    void ImageBind(unsigned int slot);
    void UnImageBind(unsigned int slot);
    void Load(const std::string path);
    unsigned int GetId();
    int GetWidth();
    int GetHeight();
    int GetDepth();
    void Clear();


private:
    unsigned int m_ID = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
};