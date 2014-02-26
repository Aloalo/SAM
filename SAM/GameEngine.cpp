#include "GameEngine.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include "Environment.h"
#include "Input.h"

using namespace glm;
using namespace std;
using namespace optix;
using namespace reng;

GameEngine::GameEngine(void)
	: player(new Player(Camera(vec3(7.0f, 9.2f, -6.0f), (float)Environment::get().screenWidth.x / Environment::get().screenHeight.x, 60.0f))),
	mouseLocked(true)
{
	tracer.initialize(drawer.createGLBuffer());
	Engine::hideMouse(true);
	//Input::addInputObserver(player);
}


GameEngine::~GameEngine(void)
{
	delete player;
}


void GameEngine::generateLabyrinth(int width, int height)
{
	lab.generateLabyrinth(width, height);
}


void GameEngine::mouseMove(double x, double y)
{
	if(mouseLocked)
		player->mouseMove(x, y);
}

void GameEngine::keyPress(int key, int scancode, int action, int mods)
{
	if(action == GLFW_REPEAT)
		return;

	if(mods == GLFW_MOD_SHIFT && action == GLFW_PRESS)
	{
		mouseLocked = false;
		Engine::hideMouse(false);
	}
	else
	{
		mouseLocked = true;
		Engine::hideMouse(true);
	}

	switch(key)
	{
	case 'L':
		if(action != GLFW_RELEASE)
		{
			lab.generateLabyrinth(lab.getWidth(), lab.getHeight());
			//tracer.createSceneGraph(lab); //TODO
		}
		break;
	}

	player->keyPress(key, scancode, action, mods);
}

void GameEngine::windowResize(int width, int height)
{
	Environment::get().screenHeight = height;
	Environment::get().screenWidth = width;
	player->windowResize(width, height);
}

void GameEngine::update(float deltaTime)
{
	player->update(deltaTime);

	tracer.setCamera(player->getCam());
}

void GameEngine::initDrawing()
{
	try
	{
		tracer.compileSceneGraph();
		tracer.setCamera(player->getCam());
		drawer.init(tracer.getBuffer());
	}
	catch(exception &ex)
	{
		puts(ex.what());
		exit(0);
	}
}

void GameEngine::draw(const glm::mat4 &View, const glm::mat4 &Projection)
{
	try
	{
		tracer.trace();
		drawer.draw(tracer.getBuffer());
	}
	catch(exception &ex)
	{
		puts(ex.what());
		exit(0);
	}
}