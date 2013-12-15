#include "Player.h"


Player::Player(const Camera &cam, float speed)
	: cam(cam), speed(speed)
{
}


Player::~Player(void)
{
}

void Player::move(float dt)
{
	cam.update(dt * speed);
}

void Player::rotate(float yaw, float pitch)
{
	cam.rotate(yaw, pitch);
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