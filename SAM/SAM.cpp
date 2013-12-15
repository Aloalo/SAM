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
	
	input.addInputObserver(std::shared_ptr<InputObserver>(game));
	e.addToUpdateList(std::shared_ptr<Updateable>(game));
	e.addToDisplayList(std::shared_ptr<Drawable>(game));
	e.start();

	return 0;
}