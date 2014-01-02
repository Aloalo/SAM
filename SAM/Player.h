#pragma once

#include "Camera.h"
#include <glm/glm.hpp>
#include "CameraHandler.h"

class Player :
	public InputObserver, public Updateable
{
public:
	Player(CameraHandler &cam);
	Player(const Camera &cam);
	~Player(void);

	void keyPress(int key, int scancode, int action, int mods);
	void mouseMove(double x, double y);
	void windowResize(int width, int height);

	void update(float deltaTime);
	const Camera& getCam() const;

private:

	CameraHandler &cam;
};

