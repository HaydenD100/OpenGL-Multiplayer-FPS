#pragma once
#include "Engine/Core/Common.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <unordered_map>

//tokyosplif code

struct Shader {
public:
    void Load(std::string vertexPath, std::string fragmentPath);
    void Load(std::string vertexPath, std::string fragmentPath, std::string geomPath);
    void Use();
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetMat4(const std::string& name, glm::mat4 value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetFloatArray(const std::string& name, const std::vector<float>& values);
    void SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values);
    int GetShaderID() const;
   
private:
    int m_ID = -1;
    std::unordered_map<std::string, int> m_uniformsLocations;
};

struct ComputeShader {
public:
    void Load(std::string computePath);
    void Use();
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec2(const std::string& name, float x, float y);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec3(const std::string& name, float x, float y, float z);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetVec4(const std::string& name, float x, float y, float z, float w);
    void SetMat2(const std::string& name, const glm::mat2& mat);
    void SetMat3(const std::string& name, const glm::mat3& mat);
    void SetMat4(const std::string& name, const glm::mat4& mat);
    void SetFloatArray(const std::string& name, const std::vector<float>& values);
    void SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values);
    int GetShaderID() const;

private:
    void CheckCompileErrors(GLuint shader, std::string type);
    int m_ID = -1;
    std::unordered_map<std::string, int> m_uniformsLocations;
};