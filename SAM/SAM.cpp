#include <cstdio>
#include "Engine.h"
#include "Input.h"
#include "GameEngine.h"

int main()
{
	int width = 800, height = 600;
	
	Engine e(1. / 60., width, height);
	Input input;

	GameEngine *game = new GameEngine(width, height);
	
	std::shared_ptr<GameEngine> ptr(game);
	input.addInputObserver(std::shared_ptr<InputObserver>(ptr));
	input.setMouseMoveCallback();
	e.addToUpdateList(std::shared_ptr<Updateable>(ptr));
	e.addToDisplayList(std::shared_ptr<Drawable>(ptr));
	e.start();
	return 0;
}