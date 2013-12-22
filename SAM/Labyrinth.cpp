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


int Labyrinth::getWidth() const
{
	return width;
}

int Labyrinth::getHeight() const
{
	return height;
}

int Labyrinth::getCell(pair<int, int> pos) const
{
	return maze[pos.first][pos.second];
}

int Labyrinth::getCell(int x, int y) const
{
	return maze[x][y];
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

	//outer walls
	boxVec.push_back(Box(make_float3(0.0f, 0.0f, -crackDim), make_float3(rw, wallHeight, 0.0f), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(-crackDim, 0.0f, 0.0f), make_float3(0.0f, wallHeight, rh), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(0.0f, 0.0f, rh), make_float3(rw, wallHeight, rh + crackDim), WALL).translated(offset));
	boxVec.push_back(Box(make_float3(rw, 0.0f, 0.0f), make_float3(rw + crackDim, wallHeight, rh), WALL).translated(offset));

	memset(maze, WALL, sizeof(int) * 200 * 200);
	primRandomized();
	generateGeometry();

	xBox = xBox.translated(-offset);
	yBox = yBox.translated(-offset);
}

bool Labyrinth::outOfBounds(int x, int y) const
{
	if(x > width || y >= height || x < 1 || y < 1)
		return true;
	return false;	
}

void Labyrinth::setWallSize(float w, float h, float d)
{
	xBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(w, h, d), WALL);
	yBox = Box(make_float3(0.0f, 0.0f, 0.0f), make_float3(d, h, w), WALL);
	
	cellDim = w;
	crackDim = d;
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

		if(t)
		{
			float3 translateVec = make_float3(minx * cellDim + (minx - 1) * crackDim, 0.0f, miny * cellDim + miny * crackDim);
			addBox(yBox.translated(translateVec, rand() % 3));
		}
		else
		{
			float3 translateVec = make_float3((minx - 1) * cellDim + (minx - 1) * crackDim, 0.0f, miny * cellDim + (miny - 1) * crackDim);
			addBox(xBox.translated(translateVec, rand() % 3));
		}
	}
	printf("%d %d\n", wallList.size() + 4, boxVec.size());
}

void Labyrinth::addBox(const Box &box)
{
	int n = boxVec.size();
	int push = 1;
	for(int i = 4; i < n; ++i)
	{
		Box b = boxVec[i];
		if(b.matIdx == box.matIdx)
		{
			if(equals(b.getDepth(), box.getDepth()) && equals(box.getDepth(), crackDim))
			{
				Box left = b.boxmin.x > box.boxmin.x ? b : box;
				Box right = b.boxmin.x < box.boxmin.x ? b : box;
				if(equals(left.boxmin.x - crackDim, right.boxmax.x) &&
					equals(left.boxmax.z, right.boxmax.z))
				{
					boxVec.erase(boxVec.begin() + i);
					boxVec.push_back(Box(right.boxmin, left.boxmax, left.matIdx));
					push = 0;
					break;
				}
			}
			else if(equals(b.getWidth(), box.getWidth()))
			{
				Box up = b.boxmin.z > box.boxmin.z ? b : box;
				Box down = b.boxmin.z < box.boxmin.z ? b : box;
				if(equals(down.boxmax.z + crackDim, up.boxmin.z) &&
					equals(down.boxmax.x, up.boxmax.x))
				{
					boxVec.erase(boxVec.begin() + i);
					boxVec.push_back(Box(down.boxmin, up.boxmax, up.matIdx));
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