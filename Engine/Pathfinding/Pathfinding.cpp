#include "Pathfinding.h"


Grid::Grid(int rows, int cols) {
	for (int x = 0; x < rows; x++) {
		std::vector<Node> col;
		for (int y = 0; y < cols; y++) {
			Node node;
			node.IsWalkable = true;
			col.push_back(node);
		}
		grid.push_back(col);
	}
}
Grid::~Grid() {

}

namespace PathFinding {

	Grid grid(50,50);

	void PathFinding::Init() {

	}
	Grid* PathFinding::GetGird() {
		return &grid;
	}

	Path PathFinding::GetPath(glm::vec3 pos) {
		
		Path path;
		//change this for some stuff
		path.start = glm::vec3(round_up(pos.x, 1), pos.y, round_up(pos.z, 1));
		
	}


}