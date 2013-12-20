#include "GameEngine.h"
#include "Engine.h"
#include <GLFW/glfw3.h>

using namespace glm;

GameEngine::GameEngine(int width, int height)
	: scene(width, height), player(Camera(vec3(7.0f, 9.2f, -6.0f), (float)width / (float)height, 60.0f), 5.0f)
{
}


GameEngine::~GameEngine(void)
{
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
	}
}

void GameEngine::mouseMove(double x, double y)
{
	if(!mouse.locked)
		return;

	int windowWidth, windowHeight;
	Engine::getWindowSize(windowWidth, windowHeight);

	//static float usex = windowWidth, usey = windowHeight;

	//float d = 1 - exp(log(0.5) * 55.f * dt);
	//usex += (x - usex) * d;
	//usey += (y - usey) * d;

	player.rotate(float(windowWidth / 2 - x) * mouse.speed, float(windowHeight / 2 - y) * mouse.speed);
	glfwSetCursorPos(Engine::getWindow(), windowWidth / 2, windowHeight / 2);
}

void GameEngine::windowResize(int width, int height)
{
	scene.setBufferSize(width, height);
	player.setAspectRatio((float) width / (float) height);
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
	scene.initialize(drawer.createGLBuffer(scene.getWidth(), scene.getHeight()));

	Labyrinth lab;
	lab.generateLabyrinth(20, 20);
	scene.createSceneGraph(lab);
	scene.setCamera(player.getCam());
	drawer.init(scene.getBuffer());
}

void GameEngine::draw(const glm::mat4 &View, const glm::mat4 &Projection)
{
	scene.trace();
	drawer.draw(scene.getBuffer());
}