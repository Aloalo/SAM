#include "GameEngine.h"
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
	const Camera& tmp = player.getCam();
	scene.setCamera(tmp.position, tmp.getDirection(), tmp.getUp(), tmp.getRight());
}

void GameEngine::initDrawing()
{
	try
	{
		scene.initialize();
	}
	catch(optix::Exception ex)
	{
		printf("%s\n", ex.what());
		exit(0);
	}
	drawer.init(scene.getBuffer());
}

void GameEngine::draw(const glm::mat4 &View, const glm::mat4 &Projection)
{
	scene.trace();
	drawer.draw(scene.getBuffer());
}