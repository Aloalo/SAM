#include "Player.h"
#include "Engine.h"
#include "Environment.h"
#include <GLFW\glfw3.h>
#include "StockCameraHandler.h"

using namespace reng;

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