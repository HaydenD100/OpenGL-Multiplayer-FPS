#include "Shader.h"

std::string readTextFromFile(std::string path) {
    std::ifstream file(path);
    std::string str;
    std::string line;
    while (std::getline(file, line)) {
        str += line + "\n";
    }
    return str;
}

int checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    return success;
}

void Shader::Load(std::string vertexPath, std::string fragmentPath) {

    std::string vertexSource = readTextFromFile(vertexPath);
    std::string fragmentSource = readTextFromFile(fragmentPath);

    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glLinkProgram(tempID);

    if (checkCompileErrors(tempID, "PROGRAM")) {
        if (m_ID != -1) {
            glDeleteProgram(m_ID);
        }
        m_ID = tempID;
        m_uniformsLocations.clear();
    }
    else {
        std::cout << "shader failed to compile " << vertexPath << " " << fragmentPath << "\n";
    }

    int InfoLogLength;
    glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(fragment, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    InfoLogLength = 0;
    glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(vertex, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }


    glDeleteShader(vertex);
    glDeleteShader(fragment);



}


void Shader::Load(std::string vertexPath, std::string fragmentPath, std::string geomPath) {
    std::string vertexSource = readTextFromFile(vertexPath);
    std::string fragmentSource = readTextFromFile(fragmentPath);
    std::string geometrySource = readTextFromFile(geomPath);

    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();
    const char* gShaderCode = geometrySource.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");

    int tempID = glCreateProgram();
    glAttachShader(tempID, vertex);
    glAttachShader(tempID, fragment);
    glAttachShader(tempID, geometry);
    glLinkProgram(tempID);

    if (checkCompileErrors(tempID, "PROGRAM")) {
        if (m_ID != -1) {
            glDeleteProgram(m_ID);
        }
        m_uniformsLocations.clear();
        m_ID = tempID;
    }
    else {
        std::cout << "shader failed to compile " << vertexPath << " " << fragmentPath << "\n";
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
}


void Shader::Use() {
    glUseProgram(m_ID);
}

void Shader::SetBool(const std::string& name, bool value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform1i(m_uniformsLocations[name], (int)value);
}

void Shader::SetInt(const std::string& name, int value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        std::cout << "Id:" << m_ID << " Couldnt find uniform: " << name << "\n";
    glUniform1i(m_uniformsLocations[name], value);
}

void Shader::SetFloat(const std::string& name, float value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform1f(m_uniformsLocations[name], value);
}

void Shader::SetMat4(const std::string& name, glm::mat4 value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniformMatrix4fv(m_uniformsLocations[name], 1, GL_FALSE, &value[0][0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform3fv(m_uniformsLocations[name], 1, &value[0]);
}
void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform2fv(m_uniformsLocations[name], 1, &value[0]);
}
void Shader::SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform3fv(m_uniformsLocations[name], (GLsizei)values.size(), glm::value_ptr(values[0]));
}
void Shader::SetFloatArray(const std::string& name, const std::vector<float>& values) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniform1fv(m_uniformsLocations[name], (GLsizei)values.size(), values.data());
}
void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    if (m_uniformsLocations.find(name) == m_uniformsLocations.end())
        m_uniformsLocations[name] = glGetUniformLocation(m_ID, name.c_str());
    //if (glGetUniformLocation(m_ID, name.c_str()) == -1)
        //std::cout << "Couldnt find uniform: " << name << "\n";
    glUniformMatrix3fv(m_uniformsLocations[name], 1, GL_FALSE, glm::value_ptr(value));
}
int Shader::GetShaderID() const {
    return m_ID;
}