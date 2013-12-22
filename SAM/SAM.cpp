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

	std::shared_ptr<GameEngine> ptr(new GameEngine());
	input.addInputObserver(std::shared_ptr<InputObserver>(ptr));
	input.setMouseMoveCallback();
	e.addToUpdateList(std::shared_ptr<Updateable>(ptr));
	e.addToDisplayList(std::shared_ptr<Drawable>(ptr));
	e.start();
	return 0;
}