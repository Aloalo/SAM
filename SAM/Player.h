#pragma once

#include "Camera.h"
#include <glm/glm.hpp>
#include "InputObserver.h"

class Player :
	public InputObserver
{
public:
	Player(const Camera &cam, float rotationSpeed, float speed);
	~Player(void);

	void keyPress(int key, int scancode, int action, int mods);
	void mouseMove(double x, double y);
	void windowResize(int width, int height);

	//move in the camera direction
	void move(float dt);

	//rotate camera by yaw and pitch
	void rotate(float yaw, float pitch);

	//set move direction in camera coordinate system
	void translate(const glm::vec3 &dir);

	void setSpeed(float newSpeed);
	const Camera& getCam() const;

private:
	float camdx, camdy;
	Camera cam;
	float rotationSpeed;
	float speed;
};

