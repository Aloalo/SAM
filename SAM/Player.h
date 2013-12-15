#pragma once

#include "Camera.h"
#include <glm/glm.hpp>

class Player
{
public:
	Player(const Camera &cam, float speed);
	~Player(void);

	//move in the camera direction
	void move(float dt);

	//rotate camera by yaw and pitch
	void rotate(float yaw, float pitch);

	//set move direction in camera coordinate system
	void translate(const glm::vec3 &dir);

	void setSpeed(float newSpeed);
	void setAspectRatio(float asratio);
	const Camera& getCam() const;

private:
	Camera cam;
	float speed;
};

