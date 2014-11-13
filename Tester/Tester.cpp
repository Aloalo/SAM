#include <cstdio>
#include <Engine.h>
#include <Input.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Macros.h>
#include <optix_math.h>
#include <VerticalLayoutManager.h>
#include <Button.h>

#include <GameEngine.h>
#include <Environment.h>
#include <Utils.h>

#include "Labyrinth.h"
#include "LabMaterials.h"

using namespace reng;
using namespace optix;
using namespace std;
using namespace trayc;
using namespace glm;

struct Junk;

GameEngine *ptr;
vector<Junk*> junk;
LabMaterials mat;

void cleanUp()
{
	for(int i = 0; i < junk.size(); ++i)
		delete junk[i];
}

struct Junk
{
	Junk()
	{
		junk.push_back(this);
	}
};

struct TintChanger : public Junk
{
	TintChanger(vec4 color) :
		color(color) {}

	void operator()()
	{
		UIManager::get().color = color;
	}

	vec4 color;
};

struct LightHandler : public Junk
{
	LightHandler(int idx, bool onoff, const float3 &color) :
		idx(idx), onoff(onoff), color(color) 
	{
	}

	void operator()()
	{
		if(onoff)
			ptr->tracer.getLight(idx).color = make_float3(0.0f);
		else
			ptr->tracer.getLight(idx).color = color;
		ptr->tracer.updateLight(idx);
		onoff = !onoff;
	}

	int idx;
	bool onoff;
	float3 color;
};

void addLabyrinth(const Labyrinth &lab)
{
	string pathFloor = Utils::pathToPTX("rectangleAA.cu");
	string pathBox = Utils::pathToPTX("box.cu");

	optix::Program boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");
	optix::Program boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");

	const vector<Box> &walls = lab.getWalls();
	int n = walls.size();
	for(int i = 0; i < n; ++i)
	{
		Geometry box = ctx->createGeometry();
		box->setPrimitiveCount(1);
		box->setBoundingBoxProgram(boxAABB);
		box->setIntersectionProgram(boxIntersect);
		box["boxmin"]->setFloat(walls[i].boxmin);
		box["boxmax"]->setFloat(walls[i].boxmax);
		ptr->tracer.addGeometryInstance(ctx->createGeometryInstance(box, &mat.getLabyrinthMaterial(walls[i].matIdx), 
			&mat.getLabyrinthMaterial(walls[i].matIdx)+1));
	}

	Geometry floor = ctx->createGeometry();
	floor->setPrimitiveCount(1);
	floor->setBoundingBoxProgram(ctx->createProgramFromPTXFile(pathFloor, "bounds"));
	floor->setIntersectionProgram(ctx->createProgramFromPTXFile(pathFloor, "intersect"));

	float rw = lab.getRealWidth(), rh = lab.getRealHeight();
	floor["plane_normal"]->setFloat(0.0f, 1.0f, 0.0f);
	floor["recmin"]->setFloat(-rw / 2.0f, 0.0f, -rh / 2.0f);
	floor["recmax"]->setFloat(rw / 2.0f, 0.0f, rh / 2.0f);

	ptr->tracer.addGeometryInstance(ctx->createGeometryInstance(floor, &mat.getLabyrinthMaterial(LabMaterials::WALL), 
		&mat.getLabyrinthMaterial(LabMaterials::WALL)+1));
}

int main()
{
	Engine e(1. / 60., Environment::get().screenWidth, Environment::get().screenHeight);
	Engine::disableMode(GL_DEPTH_TEST);
	Engine::disableMode(GL_CULL_FACE);
	Input input;

	ptr = new GameEngine();
	mat.createLabMaterials();

	try
	{
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
		ptr->tracer.addScene(mat.getLabyrinthMaterial(LabMaterials::MIRROR), nissan);*/
	}
	catch(exception &ex)
	{
		printf("%s\n", ex.what());
		return 0;
	}
	/*Labyrinth lab;
	lab.generateLabyrinth(15, 15);
	addLabyrinth(lab);*/

	e.setCamera(&ptr->player->cam);

	ptr->tracer.addLight(BasicLight(//light0 - point light
		make_float3(0.0f, 30.0f, 10.0f), //pos/dir
		make_float3(2.0f), //color
		make_float3(1.0f, 0.01f, 0.0005f),//attenuation
		make_float3(0.0f, 0.0f, 0.0f), //spot_direction
		360.0f, //spot_cutoff
		0.0f, //spot_exponent
		0.25f, //radius
		1, //casts_shadows
		0 //is_directional
		));

	ptr->tracer.addLight(BasicLight(//light1 - spot light
		make_float3(0.0f, 30.0f, 0.0f), //pos/dir
		make_float3(2.0f), //color
		make_float3(1.0f, 0.001f, 0.001f), //attenuation
		make_float3(0.0f, 0.0f, 0.0f), //spot_direction
		22.5f, //spot_cutoff
		32.0f, //spot_exponent
		0.25f, //radius
		1, //casts_shadows
		0 //is_directional
		));

	ptr->tracer.addLight(BasicLight(//light2 - directional light
		make_float3(1, 1, 1), //pos/dir
		make_float3(0.1f), //color
		make_float3(0.0f), //attenuation
		make_float3(0.0f), //spot_direction
		360.0f, //spot_cutoff
		0.0f, //spot_exponent
		0.0f, //radius
		0, //casts_shadows
		1 //is_directional
		));
	ctx->setExceptionEnabled(RT_EXCEPTION_ALL, true);


	FontFace *ff = new FontFace(Utils::defTexture("font.png"), 10, 10, ' ', 0.8);

	input.addInputObserver(ptr);
	e.addToUpdateList(ptr);
	e.addToDisplayList(ptr);

	Font *f = new Font(ff, 12);
	f->halign = Font::HAlignment::Center;
	f->valign = Font::VAlignment::Middle;

	UIManager::get().lm = new VerticalLayoutManager;

	Container *cont = new Container;
	cont->lm = new VerticalLayoutManager;
	cont->color = vec4(.4, .4, .4, 0.7);
	UIManager::get().add(cont);

	Button *b1 = new Button;
	b1->color = vec4(1, 0, 0, 1);
	b1->setAction(new LightHandler(0, true, ptr->tracer.getLight(0).color));
	b1->text = "Point";
	b1->font = f;
	cont->add(b1);

	Button *b2 = new Button;
	b2->color = vec4(0, 1, 0, 1);
	b2->setAction(new LightHandler(1, false, ptr->tracer.getLight(1).color));
	b2->text = "Spot";
	b2->font = f;
	cont->add(b2);

	Button *b3 = new Button;
	b3->color = vec4(0, 0, 1, 1);
	b3->setAction(new LightHandler(2, true, ptr->tracer.getLight(2).color));
	b3->text = "Dir";
	b3->font = f;
	cont->add(b3);

	cont->pack();
	e.start();

	delete ptr;
	delete cont;
	delete b1;
	delete b2;
	delete b3;
	delete ff;
	delete f;
	cleanUp();
	return 0;
}
