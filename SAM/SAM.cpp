#include <cstdio>
#include <Engine.h>
#include <Input.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "GameEngine.h"
#include "Environment.h"
#include "utils.h"

using namespace reng;

int main()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(utils::modelPath + "demon1_OBJ.obj", aiProcessPreset_TargetRealtime_MaxQuality);

	if(!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}

	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Input input;

	GameEngine *ptr = new GameEngine(scene);
	//GameEngine *ptr = new GameEngine();

	input.addInputObserver(ptr);
	input.setMouseMoveCallback();
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);
	e.start();

	delete ptr;
	return 0;
}