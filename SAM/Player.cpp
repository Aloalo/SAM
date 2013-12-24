#include "Player.h"


Player::Player(const Camera &cam, float speed)
	: cam(cam), speed(speed), camdx(0), camdy(0)
{
}


Player::~Player(void)
{
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
	speed = newSpeed;
}

void Player::setAspectRatio(float asratio)
{
	cam.aspectRatio = asratio;
}

const Camera& Player::getCam() const
{
	return cam;
}