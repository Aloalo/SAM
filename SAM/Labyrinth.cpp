#include "Labyrinth.h"
#include <ctime>
#include "Utils.h"

using namespace std;
using namespace utils;
using namespace optix;

Labyrinth::Labyrinth(void)
{
	setWallSize(4.3f, 7.5f, 0.15f);
}


Labyrinth::~Labyrinth(void)
{
}

int Labyrinth::getCell(pair<int, int> pos) const
{
	return maze[pos.first][pos.second];
}

int Labyrinth::getCell(int x, int y) const
{
	return maze[x][y];
}

void Labyrinth::generateLabyrinth(int w, int h)
{
	wallList.clear();
	boxVec.clear();
	width = w;
	height = h;

	float xoffset = -((float)w * cellDim) / 2.0f;
	float yoffset = -((float)h * cellDim) / 2.0f;
	xBox = xBox.translated(make_float3(xoffset, 0.0f, yoffset));
	yBox = yBox.translated(make_float3(xoffset, 0.0f, yoffset));

	memset(maze, WALL, sizeof(int) * 200 * 200);
	primRandomized();
	generateGeometry();
}

bool Labyrinth::outOfBounds(int x, int y) const
{
	if(x >= width || y >= height || x < 1 || y < 1)
		return true;
	return false;	
}

void Labyrinth::setWallSize(float w, float h, float d)
{
	xBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(-w, h, d), WALL);
	yBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(-d, h, w), WALL);
	cellDim = w;
}

void Labyrinth::primRandomized()
{
	typedef pair<int, int> pii;
	typedef pair<pii, pii> piiii;

	pii d[4] = 
	{
		pii(1, 0),
		pii(0, 1),
		pii(-1, 0),
		pii(0, -1)
	};

	srand(time(0));
	maze[1][1] = EMPTY;

	vector<piiii> walls;
	walls.push_back(piiii(pii(1, 1), pii(1, 2)));
	walls.push_back(piiii(pii(1, 1), pii(2, 1)));

	while(!walls.empty())
	{
		int k = rand() % walls.size();
		piiii wall = walls[k];
		if(getCell(wall.first) == EMPTY && getCell(wall.second) == EMPTY)
		{
			walls.erase(walls.begin() + k);
			wallList.push_back(wall);
			continue;
		}
		pii newCell = getCell(wall.first) != EMPTY ? wall.first : wall.second;
		walls.erase(walls.begin() + k);
		maze[newCell.first][newCell.second] = EMPTY;

		for(int i = 0; i < 4; ++i)
		{
			pii candidate(newCell.first + d[i].first, newCell.second + d[i].second);
			if(!outOfBounds(candidate.first, candidate.second))
				if(getCell(candidate) != EMPTY)
					walls.push_back(piiii(newCell, candidate));
		}
	}
}

void Labyrinth::generateGeometry()
{
	typedef pair<int, int> pii;
	typedef pair<pii, pii> piiii;

	int n = wallList.size();
	for(int i = 0; i < n; ++i)
	{
		piiii wall = wallList[i];

		int minx = min(wall.first.first, wall.second.first);
		int miny = min(wall.first.second, wall.second.second);
		int t = wall.first.first - wall.second.first;

		float3 translateVec = make_float3(minx * cellDim, 0.0f, miny * cellDim);
		if(t)
			boxVec.push_back(yBox.translated(translateVec, rand() % 3));
		else
			boxVec.push_back(xBox.translated(translateVec, rand() % 3));
	}
}

void Labyrinth::addBox(const Box &box)
{
	int n = boxVec.size();
	for(int i = 0; i < n; ++i)
	{
		//if(equals(boxVec[i].boxmax, box.boxmax))
	}
}

const vector<Box>& Labyrinth::getWalls() const
{
	return boxVec;
}