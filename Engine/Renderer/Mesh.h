#pragma once
#include <vector>
#include "Engine/Core/Common/RenderCommon.h"
#include "Engine/Renderer/Texture.h"



struct Mesh {
public:
    Mesh(const char* path);
    ~Mesh();
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(const Mesh&) = default;

    Mesh(std::vector<glm::vec3> vertices,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> UV,
        std::vector<unsigned short> indices,
        std::vector<glm::vec3> tangets,
        std::vector<glm::vec3> bitTangents);
    Mesh(std::vector<glm::vec3> vertices,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> UV,
        std::vector<unsigned short> indices,
        std::vector<glm::vec3> tangets,
        std::vector<glm::vec3> bitTangents,
        std::vector<glm::ivec4> jointIDs,
    std::vector<glm::vec4> weights);

    void Render(GLuint programID);
    void UploadData();
    glm::vec3 GetVertices(int index);
    std::string  GetTextureName();
    std::string GetName();
    void SetName(std::string name);
    void SetTexture(Texture* texture);
    size_t VerticiesSize();
    glm::vec3 GetVertex(int i);
    unsigned short GetIndex(int i);


    //used to bind info to a raycasting buffer, returns size of data just appended to buffer
    int BindVertices(int offset, int modelMatrixIndex);
    int BindIndices(int offset);
    void ToggleRender(bool state);


    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;
    std::vector<glm::ivec4> indexed_jointIDs;
    std::vector<glm::vec4> indexed_weights;

    std::vector<glm::vec3> vertices;

    GLuint vertexbuffer;


private:
    bool _shouldRender = true;
    std::string name;
    Texture* texture = nullptr;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;
    GLuint jointIdbuffer;
    GLuint Weightbuffer;

};