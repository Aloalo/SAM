#include "GameEngine.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include "Environment.h"
#include "Input.h"

using namespace glm;
using namespace std;
using namespace optix;
using namespace reng;

namespace trayc
{
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


	void GameEngine::mouseMove(const MouseMoveEvent &e)
	{
		if(mouseLocked)
			player->mouseMove(e);
	}

	void GameEngine::keyPress(const KeyPressEvent &e)
	{
		if(e.action == GLFW_REPEAT)
			return;

		if(e.mods == GLFW_MOD_SHIFT && e.action == GLFW_PRESS)
		{
			mouseLocked = false;
			Engine::hideMouse(false);
		}
		else
		{
			mouseLocked = true;
			Engine::hideMouse(true);
		}

		switch(e.key)
		{
		case 'L':
			if(e.action != GLFW_RELEASE)
			{
				lab.generateLabyrinth(lab.getWidth(), lab.getHeight());
				tracer.clearSceneGraph();
				tracer.addMesh(lab);
				tracer.compileSceneGraph();
			}
			break;
		}

		player->keyPress(e);
	}

	void GameEngine::windowResize(const WindowResizeEvent &e)
	{
		Environment::get().screenHeight = e.size.x;
		Environment::get().screenWidth = e.size.y;
		player->windowResize(e);
	}

	void GameEngine::update(float deltaTime)
	{
		static float timepassed = 0.0f;
		timepassed += deltaTime;

		tracer.getLight(0).pos = make_float3(0.f, 30.f, 0.f) + 5.f * make_float3(cos(timepassed), -sin(timepassed), -sin(timepassed));

		//tracer.getLight(0).spot_direction = make_float3(cos(timepassed), 0.0f, -sin(timepassed));

		tracer.updateLight(0);
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
}