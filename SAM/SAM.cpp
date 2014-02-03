#include <cstdio>
#include "Engine.h"
#include "Input.h"
#include "GameEngine.h"
#include "Environment.h"

using namespace reng;

int main()
{
	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Input input;
	
	GameEngine *ptr = new GameEngine();
	input.addInputObserver(ptr);
	input.setMouseMoveCallback();
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);
	e.start();

	delete ptr;
	return 0;
}