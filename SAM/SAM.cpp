#include <cstdio>
#include <Engine.h>
#include <Input.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Macros.h"
#include <optix_math.h>

#include "GameEngine.h"
#include "Environment.h"
#include "utils.h"

using namespace reng;

int main()
{
	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Input input;

	GameEngine *ptr = new GameEngine();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(utils::resource("crytek-sponza/sponza.obj"), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}
	ptr->tracer.addScene(utils::resource("crytek-sponza/"), scene);


	/*const aiScene* nissan = importer.ReadFile(utils::resource("nissan/nissan.obj"), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!nissan)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}
	ptr->tracer.addScene(utils::Materials::MIRROR, nissan);*/


	/*Labyrinth lab;
	lab.generateLabyrinth(15, 15);
	ptr->tracer.addMesh(lab);*/

	ptr->tracer.addLight(BasicLight(//light0 - point light
		make_float3(-5.0f, 220.0f, -16.0f), //pos/dir
		make_float3(1.0f, 1.0f, 1.0f), //color
		make_float3(1.0f, 0.001f, 0.0f), //attenuation
		make_float3(1.0f, 0.0f, 0.0f), //spot_direction
		360.0f, //spot_cutoff
		0.0f, //spot_exponent
		1, //casts_shadows
		0 //is_directional
		));;

	input.addInputObserver(ptr);
	input.setMouseMoveCallback();
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);
	e.start();

	delete ptr;
	return 0;
}