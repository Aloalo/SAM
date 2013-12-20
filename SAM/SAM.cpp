#include <cstdio>
#include "Engine.h"
#include "Input.h"
#include "GameEngine.h"
#include "GraphicsSettings.h"

int main()
{
	GraphicsSettings::readSettingsFromFile("GraphicsSettings.ini");
	
	Engine e(1. / 60., GraphicsSettings::screenWidth, GraphicsSettings::screenHeight);
	Input input;

	std::shared_ptr<GameEngine> ptr(new GameEngine());
	input.addInputObserver(std::shared_ptr<InputObserver>(ptr));
	input.setMouseMoveCallback();
	e.addToUpdateList(std::shared_ptr<Updateable>(ptr));
	e.addToDisplayList(std::shared_ptr<Drawable>(ptr));
	e.start();
	return 0;
}