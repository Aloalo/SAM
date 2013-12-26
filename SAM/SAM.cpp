#include <cstdio>
#include "Engine.h"
#include "Input.h"
#include "GameEngine.h"
#include "Settings.h"

int main()
{
	Settings::GS = GraphicsSettings("../SAM/GraphicsSettings.ini");
	
	Engine e(1. / 60., Settings::GS["screenWidth"], Settings::GS["screenHeight"]);
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