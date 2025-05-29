#include "Pathfinding.h"


Grid::Grid(int rows, int cols, glm::vec3 start) {


	for (int x = 0; x < rows; x++) {
		std::vector<Node> col;
		for (int z = 0; z < cols; z++) {
			Node node;
			node.worldpos = glm::vec3(
				(x + start.x) * PathFinding::spacing,
				start.y,
				(z + start.z) * PathFinding::spacing
			);

			node.gridpos = glm::vec3(
				(x),
				0,
				(z)
			);

			btCollisionWorld::ClosestRayResultCallback hit = PathFinding::GetRayHit(
				node.worldpos,
				glm::vec3(node.worldpos.x, 25, node.worldpos.z)
			);

			node.solid = !hit.hasHit();
			col.push_back(node);
		}
		grid.push_back(col);
	}
}

Grid::~Grid() {

}

namespace PathFinding {

	Grid grid;
	float spacing = 0.5;
	int width = 50;
	int height = 50;

	void PathFinding::Init() {
		grid = Grid(width, height, glm::vec3(-20,2,-15));

		GetPath(glm::vec3(5, 2, 5), glm::vec3(30, 2, 30));
	}
	Grid* PathFinding::GetGird() {
		return &grid;
	}

	void RenderGrid() {

	}
	bool Step(glm::vec3 start, glm::vec3 end) {
		Node* node = nullptr;

		// Find node with lowest F-value (standard A*)
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (grid.grid[x][y].GetClosed()) continue;
				if (grid.grid[x][y].parent == nullptr) continue;

				if (node == nullptr) {
					node = &grid.grid[x][y];
				}
				else if (grid.grid[x][y].f < node->f) {
					node = &grid.grid[x][y];
				}
				// Optional: Break ties with H-value
				else if (grid.grid[x][y].f == node->f &&
					grid.grid[x][y].h < node->h) {
					node = &grid.grid[x][y];
				}
			}
		}

		if (node->gridpos.x == end.x && node->gridpos.y == end.z) {
			std::cout << "Path found!\n";
			GetPath(end, start);
			return false;
		}
		else {
			node->closed = true;
			calculateNeighbours(node->gridpos.x, node->gridpos.y, start, end);
		}
	}
	Path GetPath(glm::vec3 start, glm::vec3 end) {
		Path path;
		//change this for some stuff
		path.start = glm::vec3(round_up(start.x, 1), start.y, round_up(start.z, 1));

		// Initialize starting point's cost values
		grid.grid[start.x][start.z].h = glm::distance(grid.grid[start.x][start.z].gridpos, end) * 10;

		grid.grid[start.x][start.z].g = glm::distance(grid.grid[start.x][start.z].gridpos, start) * 10;

		grid.grid[start.x][start.z].f = grid.grid[start.x][start.z].g + grid.grid[start.x][start.z].h;
		//Initializes the starting point's parent property to itself
		grid.grid[start.x][start.z].parent = &grid.grid[start.x][start.z];
		grid.grid[start.x][start.z].closed = 0;

		bool start_solving = true;
		/*
		while (Step(start, end)) {
			
		}
		*/
		std::cout << "Pathing finished \n";

		return path;
	}

	btCollisionWorld::ClosestRayResultCallback GetRayHit(glm::vec3 start, glm::vec3 out_end) {

		btCollisionWorld::ClosestRayResultCallback RayCallback(
			btVector3(start.x, start.y, start.z),
			btVector3(out_end.x, out_end.y, out_end.z)
		);
		RayCallback.m_collisionFilterGroup = GROUP_PLAYER;
		RayCallback.m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
		PhysicsManagerBullet::GetDynamicWorld()->rayTest(btVector3(start.x, start.y, start.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
		return RayCallback;
	}


	void calculateNode(Node* node, int x, int y, glm::vec3 start, glm::vec3 end) {

		// Skip solid nodes and closed nodes
		if (node->solid || grid.grid[x][y].closed) {
			return;
		}

		// Compute heuristic and cost values
		grid.grid[x][y].h = glm::distance(grid.grid[x][y].gridpos, end) * 10;

		grid.grid[x][y].g = glm::distance(grid.grid[x][y].gridpos, start) * 10;

		grid.grid[x][y].f = grid.grid[x][y].g + grid.grid[x][y].h;

		// Update the parent node if this path is shorter
		if (grid.grid[x][y].parent != nullptr) {
			if (node->f < grid.grid[x][y].parent->f && node->h < grid.grid[x][y].parent->h) {
				grid.grid[x][y].parent = node;
			}
		}
		else {
			grid.grid[x][y].parent = node; // Set parent if none exists
		}
	}

	void calculateNeighbours(int x, int y, glm::vec3 start, glm::vec3 end) {

		// Check neighboring cells (left, right, up, down)
		if (x > 0) {
			calculateNode(&grid.grid[x][y], x - 1, y, start,end); // Left
		}
		if (x + 1 < width) {
			calculateNode(&grid.grid[x][y], x + 1, y, start, end); // Right
		}
		if (y > 0) {
			calculateNode(&grid.grid[x][y], x, y - 1, start, end); // Up
		}
		if (y + 1 < width) {
			calculateNode(&grid.grid[x][y], x, y + 1, start, end); // Down
		}

		// Check diagonal neighbors
		if (x > 0 && y > 0) {
			calculateNode(&grid.grid[x][y], x - 1, y - 1, start, end); // Top-left
		}
		if (x + 1 < width && y + 1 < height) {
			calculateNode(&grid.grid[x][y], x + 1, y + 1, start, end); // Bottom-right
		}
		if (x + 1 < width && y > 0) {
			calculateNode(&grid.grid[x][y], x + 1, y - 1, start, end); // Top-right
		}
		if (x > 0 && y + 1 < height) {
			calculateNode(&grid.grid[x][y], x - 1, y + 1, start, end); // Bottom-left
		}
	}

	// Retrieve the shortest path from the endpoint to the starting point
	void GetPath(int x, int y, glm::vec3 start) {
		if (x == start.x && y == start.z)
			return;
		if (grid.grid[x][y].parent->parent == &grid.grid[x][y])
			return;
		grid.grid[x][y].path = true;
		GetPath(grid.grid[x][y].parent->gridpos.x, grid.grid[x][y].parent->gridpos.z,start);
	}



}