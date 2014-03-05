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

		void keyPress(int key, int scancode, int action, int mods);
		void mouseMove(double x, double y);
		void windowResize(int width, int height);

		void update(float deltaTime);

		void initDrawing();
		void draw(const glm::mat4 &View, const glm::mat4 &Projection);

		void generateLabyrinth(int width, int height);

		OptixTracer tracer;
	private:
		BufferDrawer drawer;
		Player *player;
		Labyrinth lab;
		bool mouseLocked;
	};
}
