#pragma once
#include "Engine/Core/Common.h"
#include "Engine/Core/GameObject.h"
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
	bool IsWalkable = true;
    glm::vec3 worldpos;
};

struct Grid {
    Grid(int rows, int cols);
    ~Grid();
    std::vector<std::vector<Node>> grid;

private:
    int rows, cols;
};


namespace PathFinding {
	void Init();
    Grid* GetGird();
    Path GetPath(glm::vec3 pos);

}