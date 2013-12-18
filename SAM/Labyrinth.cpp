//#include "Labyrinth.h"
//#include "Utils.h"
//#include <cstring>
//#include <ctime>
//
//using namespace utils;
//using namespace std;
//
//Labyrinth::Labyrinth(void)
//{
//}
//
//
//Labyrinth::~Labyrinth(void)
//{
//}
//
//int Labyrinth::getCell(std::pair<int, int> pos)
//{
//	return maze[pos.first][pos.second];
//}
//
//int Labyrinth::getCell(int x, int y)
//{
//	return maze[x][y];
//}
//
//int Labyrinth::getWall(std::pair<std::pair<int, int>, std::pair<int, int> > pos)
//{
//	return 0;
//}
//
//int Labyrinth::getWall(int x1, int y1, int x2, int y2)
//{
//	return 0;
//}
//
//void Labyrinth::generateLabyrinth(int w, int h)
//{
//	width = w;
//	height = h;
//	memset(maze, WALL, sizeof(maze));
//	primRandomized();
//}
//
//
//void Labyrinth::primRandomized()
//{
//	typedef pair<int, int> pii;
//	typedef pair<pii, pii> piiii;
//
//	pii d[4] = 
//	{
//		pii(1, 0),
//		pii(0, 1),
//		pii(-1, 0),
//		pii(0, -1)
//	};
//
//	srand(time(0));
//	maze[1][1] = EMPTY;
//	wallList.push_back(piiii(pii(1, 1), pii(1, 2)));
//	wallList.push_back(piiii(pii(1, 1), pii(2, 1)));
//
//	while(!wallList.empty())
//	{
//		int k = rand() % wallList.size();
//		piiii wall = wallList[k];
//		//if(maze[wall.first)
//	}
//}