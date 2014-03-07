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
	}

	void Player::keyPress(const KeyPressEvent &e)
	{
		if(e.key == GLFW_KEY_9 && e.action == GLFW_PRESS)
			((StockCameraHandler*)&cam)->speed *= 2.0f;
		else if(e.key == GLFW_KEY_0 && e.action == GLFW_PRESS)
			((StockCameraHandler*)&cam)->speed /= 2.0f;
		else
			cam.keyPress(e);
	}

	void Player::mouseMove(const MouseMoveEvent &e)
	{
		cam.mouseMove(e);
	}

	void Player::windowResize(const WindowResizeEvent &e)
	{
		cam.windowResize(e);
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