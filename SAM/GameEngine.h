#pragma once

#include "UnLitObject.h"
#include "InputObserver.h"
#include "Updateable.h"
#include "Scene.h"
#include "BufferDrawer.h"
#include "Player.h"
#include "Mouse.h"

class GameEngine :
	public UnLitObject, public Updateable, public InputObserver
{
public:
	GameEngine(int width, int height);
	~GameEngine(void);

	void keyPress(int key, int scancode, int action, int mods);
	void mouseMove(double x, double y);
	void windowResize(int width, int height);

	void initState();
	void update(float deltaTime);

	void initDrawing();
	void draw(const glm::mat4 &View, const glm::mat4 &Projection);

	Mouse mouse;

private:
	Scene scene;
	BufferDrawer drawer;
	Player player;
};

