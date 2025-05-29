#pragma once
#include "Engine/Core/Common.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Physics/BulletPhysics.h"
#define NodeSize = 0.1;



struct Path {
    glm::vec3 start;
    glm::vec3 end;
    std::vector<glm::vec3> points;
    bool Found() {
        return points.size() >= 2;
    }
};

struct Node {
    int solid = 0;           // Indicates if the node is solid (obstacle)
    int end_goal = 0;       // Flag for the end goal node
    int start_point = 0;    // Flag for the starting point node
    int g = 0;              // Cost from start to this node
    glm::vec3 worldpos;
    glm::vec3 gridpos;
    int h = 0;              // Heuristic cost to the end goal
    int f = 0;              // Total cost (g + h)
    int closed = 0;         // Indicates if the node has been evaluated
    int path = 0;           // Flag for the final path

    Node* parent = nullptr; // Pointer to the parent node

    int GetClosed() {
        return closed;      // Return the closed status of the node
    }

};

struct Grid {
    Grid() = default;
    Grid(int rows, int cols, glm::vec3 start);
    ~Grid();
    std::vector<std::vector<Node>> grid;

private:
    int rows, cols;
};


namespace PathFinding {
    extern float spacing;

	void Init();
    Grid* GetGird();
    Path GetPath(glm::vec3 start, glm::vec3 end);

    btCollisionWorld::ClosestRayResultCallback GetRayHit(glm::vec3 start, glm::vec3 out_end);

    //A*
    void calculateNode(Node* node, int x, int y, glm::vec3 start, glm::vec3 end);
    void calculateNeighbours(int x, int y, glm::vec3 start, glm::vec3 end);
    void GetPath(int x, int y, glm::vec3 start);
    bool Step(glm::vec3 start, glm::vec3 end);

}