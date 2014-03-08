#pragma once

#include <Camera.h>
#include <glm/glm.hpp>
#include <CameraHandler.h>

namespace trayc
{
	class Player :
		public reng::InputObserver, public reng::Updateable
	{
	public:
		Player(reng::CameraHandler &cam);
		Player(const reng::Camera &cam);
		~Player(void);

		void keyPress(const reng::KeyPressEvent &e);
		void mouseMove(const reng::MouseMoveEvent &e);
		void windowResize(const reng::WindowResizeEvent &e);

		void update(float deltaTime);
		const reng::Camera& getCam() const;

		reng::CameraHandler &cam;
	};
}
