#pragma once

#include <UnLitObject.h>
#include "Player.h"
#include "OptixTracer.h"
#include "BufferDrawer.h"

namespace trayc
{
	class GameEngine :
		public reng::UnLitObject, public reng::Updateable, public reng::InputObserver
	{
	public:
		GameEngine(void);
		~GameEngine(void);

		void mouseClick(const reng::MouseClickEvent &e);
		void keyPress(const reng::KeyPressEvent &e);
		void mouseMove(const reng::MouseMoveEvent &e);
		void windowResize(const reng::WindowResizeEvent &e);

		void update(float deltaTime);

		void initDrawing();
		void draw(const glm::mat4 &View, const glm::mat4 &Projection);

		Player *player;
		OptixTracer tracer;
	private:
		BufferDrawer drawer;
		bool mouseLocked;
	};
}
