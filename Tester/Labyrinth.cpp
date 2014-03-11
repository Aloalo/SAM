#include "Labyrinth.h"
#include <ctime>
#include "LabMaterials.h"
#include <Utils.h>

using namespace std;
using namespace optix;
using namespace trayc;

Labyrinth::Labyrinth(void)
	: width(15), height(15)
{
	setWallSize(4.3f, 7.5f, 0.15f);
}


Labyrinth::~Labyrinth(void)
{
}


int Labyrinth::getWidth() const
{
	return width;
}

int Labyrinth::getHeight() const
{
	return height;
}

float Labyrinth::getRealWidth() const
{
	return (float)width * cellDim + (float)(width - 1.0f) * crackDim;
}

float Labyrinth::getRealHeight() const
{
	return (float)height * cellDim + (float)(height - 1.0f) * crackDim;
}

void Labyrinth::generateLabyrinth(int w, int h)
{
	wallList.clear();
	boxVec.clear();
	width = w;
	height = h;

	float rw = getRealWidth();
	float rh = getRealHeight();
	float3 offset = make_float3(-rw / 2.0f, 0.0f, -rh / 2.0f);
	xBox = xBox.translated(offset);
	yBox = yBox.translated(offset);
	float wallHeight = xBox.getHeight();

	int WALL = LabMaterials::WALL;

	//outer walls
	boxVec.push_back(Box(make_float3(0.0f, 0.0f, -crackDim), make_float3(rw, wallHeight, 0.0f), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(-crackDim, 0.0f, 0.0f), make_float3(0.0f, wallHeight, rh), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(0.0f, 0.0f, rh), make_float3(rw, wallHeight, rh + crackDim), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(rw, 0.0f, 0.0f), make_float3(rw + crackDim, wallHeight, rh), WALL).translated(offset));

	memset(maze, 0, sizeof(maze));
	primRandomized();
	generateGeometry();

	xBox = xBox.translated(-offset);
	yBox = yBox.translated(-offset);
}

bool Labyrinth::outOfBounds(int x, int y) const
{
	return x >= width || y >= height || x < 0 || y < 0;
}

void Labyrinth::setWallSize(float w, float h, float d)
{
	int WALL = LabMaterials::WALL;
	xBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(w, h, d), WALL);
	yBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(d, h, w), WALL);

	cellDim = w;
	crackDim = d;
}

char& getCell(vector<vector<char>> &maze, pair<int, int> pos)
{
	return maze[pos.first][pos.second];
}

void Labyrinth::primRandomized()
{
	typedef pair<int, int> pii;
	typedef pair<pii, pii> piiii;

	int WALL = LabMaterials::WALL;
	int EMPTY = 0;


	pii d[4] = 
	{
		pii(1, 0),
		pii(0, 1),
		pii(-1, 0),
		pii(0, -1)
	};

	vector<vector<char>> maze(height);
	for(int i = 0; i < height; ++i)
		maze[i] = vector<char>(width, WALL);

	srand(time(0));
	maze[0][0] = EMPTY;

	vector<piiii> walls;
	walls.push_back(piiii(pii(0, 0), pii(0, 1)));
	walls.push_back(piiii(pii(0, 0), pii(1, 0)));

	while(!walls.empty())
	{
		int k = rand() % walls.size();
		piiii wall = walls[k];
		if(getCell(maze, wall.second) == EMPTY)
		{
			walls.erase(walls.begin() + k);
			wallList.push_back(wall);
			continue;
		}
		pii newCell = wall.second;
		walls.erase(walls.begin() + k);
		getCell(maze, newCell) = EMPTY;
		maze[newCell.first][newCell.second] = EMPTY;

		for(int i = 0; i < 4; ++i)
		{
			pii candidate(newCell.first + d[i].first, newCell.second + d[i].second);
			if(!outOfBounds(candidate.first, candidate.second))
				if(getCell(maze, candidate) == WALL)
					walls.push_back(piiii(newCell, candidate));
		}
	}
}

void Labyrinth::generateGeometry()
{
	typedef pair<int, int> pii;
	typedef pair<pii, pii> piiii;

	memset(maze, 0, sizeof maze);
	int n = wallList.size();
	for(int i = 0; i < n; ++i)
	{
		piiii wall = wallList[i];

		int minx = min(wall.first.first, wall.second.first);
		int miny = min(wall.first.second, wall.second.second);
		int t = wall.first.first - wall.second.first;

		if(t) //vertikalni
		{
			maze[minx][miny].walls[3] = 1;
			maze[minx+1][miny].walls[1] = 1;
			float3 translateVec = make_float3((minx + 1) * cellDim + minx * crackDim, 0.0f, miny * cellDim + miny * crackDim);
			addBox(yBox.translated(translateVec, rand() % 3));
		}
		else //horizontalni
		{
			maze[minx][miny].walls[0] = 1;
			maze[minx][miny+1].walls[2] = 1;
			float3 translateVec = make_float3(minx * cellDim + minx * crackDim, 0.0f, (miny + 1) * cellDim + miny * crackDim);
			addBox(xBox.translated(translateVec, rand() % 3));
		}
	}
	printf("%d %d\n", wallList.size() + 4, boxVec.size());
}

void Labyrinth::addBox(const Box &box)
{
	int n = boxVec.size();
	int push = 1;
	for(int i = 0; i < n; ++i)
	{
		Box b = boxVec[i];
		if(b.matIdx == box.matIdx)
		{
			if(Utils::equals(b.getDepth(), box.getDepth()) && Utils::equals(box.getDepth(), crackDim))
			{
				Box left = b.boxmin.x > box.boxmin.x ? b : box;
				Box right = b.boxmin.x < box.boxmin.x ? b : box;
				if(Utils::equals(left.boxmin.x - crackDim, right.boxmax.x) &&
					Utils::equals(left.boxmax.z, right.boxmax.z))
				{
					boxVec.erase(boxVec.begin() + i);
					Box newBox(right.boxmin, left.boxmax, left.matIdx);
					addBox(newBox);
					push = 0;
					break;
				}
			}
			else if(Utils::equals(b.getWidth(), box.getWidth()))
			{
				Box up = b.boxmin.z > box.boxmin.z ? b : box;
				Box down = b.boxmin.z < box.boxmin.z ? b : box;
				if(Utils::equals(down.boxmax.z + crackDim, up.boxmin.z) &&
					Utils::equals(down.boxmax.x, up.boxmax.x))
				{
					boxVec.erase(boxVec.begin() + i);
					Box newBox(down.boxmin, up.boxmax, up.matIdx);
					addBox(newBox);
					push = 0;
					break;
				}
			}
		}
	}
	if(push)
		boxVec.push_back(box);
}

const vector<Box>& Labyrinth::getWalls() const
{
	return boxVec;
}