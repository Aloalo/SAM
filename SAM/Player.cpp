#include "Player.h"
#include "Engine.h"
#include <GLFW\glfw3.h>

Player::Player(const Camera &cam, float rotationSpeed, float speed)
	: cam(cam), rotationSpeed(rotationSpeed), speed(speed), camdx(0), camdy(0)
{

}


Player::~Player(void)
{
}

void Player::keyPress(int key, int scancode, int action, int mods)
{
	if(action == GLFW_REPEAT)
		return;

	float mod = action == GLFW_PRESS ? 1.0f : -1.0f;
	switch(key)
	{
	case 'W':
		translate(glm::vec3(0, 0, 1.) * mod);
		break;
	case 'A':
		translate(glm::vec3(-1., 0, 0) * mod);
		break;
	case 'S':
		translate(glm::vec3(0, 0, -1.) * mod);
		break;
	case 'D':
		translate(glm::vec3(1., 0, 0) * mod);
		break;
	case 'Q':
		translate(glm::vec3(0, -1., 0) * mod);
		break;
	case 'E':
		translate(glm::vec3(0, 1., 0) * mod);
		break;
	}
}

void Player::mouseMove(double x, double y)
{
	int windowWidth, windowHeight;
	Engine::getWindowSize(windowWidth, windowHeight);

	rotate(float(windowWidth / 2 - x) * rotationSpeed, float(windowHeight / 2 - y) * rotationSpeed);
	glfwSetCursorPos(Engine::getWindow(), windowWidth / 2, windowHeight / 2);
}

void Player::windowResize(int width, int height)
{
	cam.aspectRatio = (float) width / height;
}

void Player::move(float dt)
{
	float cf = 1 - exp(-100 * dt);
	float dxr = cf * camdx;
	float dyr = cf * camdy;
	cam.rotate(dxr, dyr);
	camdx -= dxr;
	camdy -= dyr;
	cam.update(dt * speed);
}

void Player::rotate(float yaw, float pitch)
{
	camdx += yaw;
	camdy += pitch;
}

void Player::translate(const glm::vec3 &dir)
{
	cam.translate(dir);
}

void Player::setSpeed(float newSpeed)
{
	rotationSpeed = newSpeed;
}

const Camera& Player::getCam() const
{
	return cam;
}