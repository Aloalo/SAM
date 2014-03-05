#pragma once

#include <vector>
#include "Box.h"

namespace trayc
{
	class Labyrinth
	{
	public:
		Labyrinth(void);
		~Labyrinth(void);

		void generateLabyrinth(int w, int h);

		//wall width, height and depth
		void setWallSize(float w, float h, float d);
		const std::vector<Box>& getWalls() const;
		float getRealWidth() const;
		float getRealHeight() const;
		int getWidth() const;
		int getHeight() const;

		struct Cell
		{
			bool walls[4];
		};

	private:
		//generates a list of walls
		void primRandomized();

		//generates the maze geometry
		void generateGeometry();

		//adds a box to the list. merges with others if possible
		void addBox(const Box &box);

		bool outOfBounds(int x, int y) const;

		Box xBox, yBox;
		float cellDim, crackDim;
		int width, height;
		Cell maze[200][200];
		std::vector<std::pair<std::pair<int, int>, std::pair<int, int> > > wallList;
		std::vector<Box> boxVec;
	};
}
