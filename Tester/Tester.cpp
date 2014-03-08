#include <cstdio>
#include <Engine.h>
#include <Input.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Macros.h>
#include <optix_math.h>
#include <VerticalLayoutManager.h>
#include <Button.h>

#include "GameEngine.h"
#include "Environment.h"
#include "Utils.h"

using namespace reng;
using namespace trayc;
using namespace glm;

struct TintChanger
{
	TintChanger(vec4 color) :
		color(color) {}

	void operator()()
	{
		UIManager::get().color = color;
	}

	vec4 color;
};

void blah()
{
	puts("trolololo");
}

int main()
{
	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Engine::disableMode(GL_DEPTH_TEST);
	Engine::disableMode(GL_CULL_FACE);
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
	e.setCamera(&ptr->player->cam);

	/*const aiScene* nissan = importer.ReadFile(Utils::resource("nissan/nissan.obj"), aiProcessPreset_TargetRealtime_MaxQuality);
	if(!nissan)
	{
		printf("%s\n", importer.GetErrorString());
		return 0;
	}
	ptr->tracer.addScene(MaterialHandler::LabMaterials::MIRROR, nissan);*/

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
	//input.setMouseMoveCallback();
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);

	UIManager::get().lm = new VerticalLayoutManager;

	Container *cont = new Container;
	cont->lm = new VerticalLayoutManager;
	cont->color = vec4(.4, .4, .4, 0.7);
	UIManager::get().add(cont);
	
	Button *b1 = new Button;
	b1->color = vec4(1, 0, 0, 1);
	b1->setAction(new TintChanger(vec4(1, 0, 0, 0.2)));
	cont->add(b1);

	Button *b2 = new Button;
	b2->color = vec4(0, 1, 0, 1);
	b2->setAction(new TintChanger(vec4(0, 1, 0, 0.2)));
	cont->add(b2);

	Button *b3 = new Button;
	b3->color = vec4(0, 0, 1, 1);
	b3->setAction(new TintChanger(vec4(0, 0, 1, 0.2)));
	cont->add(b3);

	Button *b4 = new Button;
	b4->color = vec4(1, 1, 1, 1);
	b4->setAction(new TintChanger(vec4(1, 1, 1, 0)));
	cont->add(b4);

	Button *b5 = new Button;
	b5->d = vec2(150, 150);
	b5->color = vec4(1, 0, 1, 1);
	b5->setAction(blah);
	cont->add(b5);

	cont->pack();

	e.start();

	delete ptr;
	return 0;
}