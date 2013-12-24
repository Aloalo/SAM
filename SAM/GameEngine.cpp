#include "GameEngine.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include "Settings.h"

using namespace glm;
using namespace std;
using namespace optix;

GameEngine::GameEngine(void)
	: player(Camera(vec3(7.0f, 9.2f, -6.0f), (float)Settings::GS["screenWidth"] / (float)Settings::GS["screenHeight"], 60.0f), 5.0f)
{
}


GameEngine::~GameEngine(void)
{
}


void GameEngine::generateLabyrinth(int width, int height)
{
	lab.generateLabyrinth(width, height);
}

void GameEngine::keyPress(int key, int scancode, int action, int mods)
{
	if(action == GLFW_REPEAT)
		return;

	if(mods == GLFW_MOD_SHIFT && action == GLFW_PRESS)
	{
		mouse.locked = false;
		glfwSetInputMode(Engine::getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		mouse.locked = true;
		glfwSetInputMode(Engine::getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	float mod = action == GLFW_PRESS? 1.0f : -1.0f;
	switch(key)
	{
	case 'W':
		player.translate(glm::vec3(0, 0, 1.) * mod);
		break;
	case 'A':
		player.translate(glm::vec3(-1., 0, 0) * mod);
		break;
	case 'S':
		player.translate(glm::vec3(0, 0, -1.) * mod);
		break;
	case 'D':
		player.translate(glm::vec3(1., 0, 0) * mod);
		break;
	case 'Q':
		player.translate(glm::vec3(0, -1., 0) * mod);
		break;
	case 'E':
		player.translate(glm::vec3(0, 1., 0) * mod);
		break;
	case 'L':
		if(action != GLFW_RELEASE)
		{
			lab.generateLabyrinth(lab.getWidth(), lab.getHeight());
			scene.createSceneGraph(lab);
		}
		break;
	}
}

void GameEngine::mouseMove(double x, double y)
{
	if(!mouse.locked)
		return;

	int windowWidth, windowHeight;
	Engine::getWindowSize(windowWidth, windowHeight);

	player.rotate(float(windowWidth / 2 - x) * mouse.speed, float(windowHeight / 2 - y) * mouse.speed);
	glfwSetCursorPos(Engine::getWindow(), windowWidth / 2, windowHeight / 2);
}

void GameEngine::windowResize(int width, int height)
{
	player.setAspectRatio((float) width / (float) height);
	Settings::GS["screenHeight"] = height;
	Settings::GS["screenWidth"] = width;
}

void GameEngine::initState()
{
}

void GameEngine::update(float deltaTime)
{
	player.move(deltaTime);
	scene.setCamera(player.getCam());
}

void GameEngine::initDrawing()
{
	try
	{
		scene.initialize(drawer.createGLBuffer(Settings::GS["bufferWidth"], Settings::GS["bufferHeight"]));
		lab.generateLabyrinth(20, 20);

		scene.createSceneGraph(lab);
		scene.setCamera(player.getCam());
		drawer.init(scene.getBuffer());
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
		scene.trace();
		drawer.draw(scene.getBuffer());
	}
	catch(exception &ex)
	{
		puts(ex.what());
		exit(0);
	}
}