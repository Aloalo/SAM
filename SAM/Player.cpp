#include "Player.h"
#include <Engine.h>
#include "Environment.h"
#include <GLFW\glfw3.h>
#include <StockCameraHandler.h>

using namespace reng;

namespace trayc
{
	Player::Player(CameraHandler &cam)
		: cam(cam)
	{
	}

	Player::Player(const Camera &cam)
		: cam(*(new StockCameraHandler(cam, 7.0f, 0.008f)))
	{
	}

	Player::~Player(void)
	{
		delete &cam;
	}

	void Player::keyPress(int key, int scancode, int action, int mods)
	{
		if(key == GLFW_KEY_9 && action == GLFW_PRESS)
			((StockCameraHandler*)&cam)->speed *= 2.0f;
		else if(key == GLFW_KEY_0 && action == GLFW_PRESS)
			((StockCameraHandler*)&cam)->speed /= 2.0f;
		else
			cam.keyPress(key, scancode, action, mods);
	}

	void Player::mouseMove(double x, double y)
	{
		cam.mouseMove(x, y);
	}

	void Player::windowResize(int width, int height)
	{
		cam.windowResize(width, height);
	}

	const Camera& Player::getCam() const
	{
		return cam.cam;
	}


	void Player::update(float deltaTime)
	{
		cam.update(deltaTime);
	}
}