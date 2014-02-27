#pragma once

#include <Camera.h>
#include <glm/glm.hpp>
#include <CameraHandler.h>

class Player :
	public reng::InputObserver, public reng::Updateable
{
public:
	Player(reng::CameraHandler &cam);
	Player(const reng::Camera &cam);
	~Player(void);

	void keyPress(int key, int scancode, int action, int mods);
	void mouseMove(double x, double y);
	void windowResize(int width, int height);

	void update(float deltaTime);
	const reng::Camera& getCam() const;

private:

	reng::CameraHandler &cam;
};

