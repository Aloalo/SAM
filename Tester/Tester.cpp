#include <cstdio>
#include <Engine.h>
#include <Input.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Macros.h>
#include <optix_math.h>

#include "GameEngine.h"
#include "Environment.h"
#include "Utils.h"

using namespace reng;
using namespace trayc;

int main()
{
	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Input input;

	GameEngine *ptr = new GameEngine();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(Utils::resource("crytek-sponza/sponza.obj"), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}
	ptr->tracer.addScene(Utils::resource("crytek-sponza/"), scene);


	/*const aiScene* nissan = importer.ReadFile(Utils::resource("nissan/nissan.obj"), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!nissan)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}
	ptr->tracer.addScene(MaterialHandler::LabMaterials::MIRROR, nissan);
	*/

	/*Labyrinth lab;
	lab.generateLabyrinth(150, 150);
	ptr->tracer.addMesh(lab);*/

	ptr->tracer.addLight(BasicLight(//light0 - point light
		make_float3(0.0f, 30.0f, 10.0f), //pos/dir
		make_float3(1.5f), //color
		make_float3(1.0f, 0.01f, 0.0005f), //attenuation
		make_float3(0.0f, 0.0f, 0.0f), //spot_direction
		360.0f, //spot_cutoff
		0.0f, //spot_exponent
		1, //casts_shadows
		0 //is_directional
		));

	//ptr->tracer.addLight(BasicLight(//light1 - spot light
	//	make_float3(0.0f, 30.0f, 0.0f), //pos/dir
	//	make_float3(2.0f), //color
	//	make_float3(1.0f, 0.0f, 0.01f), //attenuation
	//	make_float3(1.0f, 0.0f, 0.0f), //spot_direction
	//	22.5f, //spot_cutoff
	//	5.0f, //spot_exponent
	//	1, //casts_shadows
	//	0 //is_directional
	//	));

	ptr->tracer.addLight(BasicLight(//light2 - directional light
		make_float3(1, 1, 1), //pos/dir
		make_float3(0.1f), //color
		make_float3(0.0f), //attenuation
		make_float3(0.0f), //spot_direction
		360.0f, //spot_cutoff
		0.0f, //spot_exponent
		0, //casts_shadows
		1 //is_directional
		));

	input.addInputObserver(ptr);
	input.setMouseMoveCallback();
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);
	e.start();

	delete ptr;
	return 0;
}