#include "Scene.h"
#include "Utils.h"
#include "lights.h"
#include "Settings.h"

using namespace optix;
using namespace utils;
using namespace glm;
using namespace std;

Scene::Scene(void)
{
}


Scene::~Scene(void)
{
	//ctx->destroy();
}

Buffer Scene::getBuffer()
{
	return ctx["output_buffer"]->getBuffer();
}

void Scene::setBufferSize(int w, int h)
{
	w = max(1, w);
	w = min(w, Settings::GS["maxBufferWidth"]);
	
	h = max(1, h);
	h = min(h, Settings::GS["maxBufferHeight"]);
	ctx["output_buffer"]->getBuffer()->setSize(w, h);
}

void Scene::initialize(unsigned int GLBO)
{
	ctx = Context::create();

	ctx->setRayTypeCount(2);
	ctx->setEntryPointCount(1);
	ctx->setStackSize(4640);

	ctx["radiance_ray_type"]->setUint(0);
	ctx["shadow_ray_type"]->setUint(1);
	ctx["scene_epsilon"]->setFloat(1.e-3f);
	ctx["max_depth"]->setInt(4);
	ctx["importance_cutoff"]->setFloat(0.01f);
	ctx["ambient_light_color"]->setFloat(0.3f, 0.3f, 0.3f);
	Buffer buff;
	if(Settings::GS["useVBO"])
	{
		buff = ctx->createBufferFromGLBO(RT_BUFFER_OUTPUT, GLBO);
		buff->setSize(Settings::GS["bufferWidth"], Settings::GS["bufferHeight"]);
		buff->setFormat((RTformat)Settings::GS["bufferFormat"]);
	}
	else
		buff = ctx->createBuffer(RT_BUFFER_OUTPUT, (RTformat)Settings::GS["bufferFormat"], Settings::GS["bufferWidth"], Settings::GS["bufferHeight"]);

	ctx["output_buffer"]->setBuffer(buff);


	std::string path = pathToPTX("shaders.cu");

	ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(path, "pinhole_camera"));

	ctx->setExceptionProgram(0, ctx->createProgramFromPTXFile(path, "exception"));
	ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

	ctx->setMissProgram(0, ctx->createProgramFromPTXFile(path, "gradient_miss"));
	ctx["miss_min"]->setFloat(0.3f, 0.3f, 0.3f);
	ctx["miss_max"]->setFloat(0.8f, 0.8f, 0.8f);

	BasicLight lights[] = 
	{ 
		{make_float3(-5.0f, 60.0f, -16.0f), make_float3(1.0f, 1.0f, 1.0f), 1}
	};

	Buffer lightBuffer = ctx->createBuffer(RT_BUFFER_INPUT);
	lightBuffer->setFormat(RT_FORMAT_USER);
	lightBuffer->setElementSize(sizeof(BasicLight));
	lightBuffer->setSize(sizeof(lights) / sizeof(lights[0]));
	memcpy(lightBuffer->map(), lights, sizeof(lights));
	lightBuffer->unmap();

	ctx["lights"]->set(lightBuffer);

	createMaterials();
}

void Scene::createMaterials()
{
	std::string mainPath(pathToPTX("shaders.cu"));
	Program phongClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_phong");
	Program tileClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_phong_tile");
	Program shadowAnyHit = ctx->createProgramFromPTXFile(mainPath, "any_hit_solid");
	Program glassAnyHit = ctx->createProgramFromPTXFile(mainPath, "any_hit_shadow_glass");
	Program mirrorClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_reflection");
	Program glassClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_glass");

	Material floorMaterial = ctx->createMaterial();
	floorMaterial->setClosestHitProgram(0, tileClosestHit);
	floorMaterial->setAnyHitProgram(1, shadowAnyHit);

	floorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	floorMaterial["Kd"]->setFloat(0.6f, 0.7f, 0.8f);
	floorMaterial["Ks"]->setFloat(0.0f, 0.0f, 0.0f);
	floorMaterial["phong_exp"]->setFloat(0.0f);
	floorMaterial["tile_v0"]->setFloat(0.25f, 0, .15f);
	floorMaterial["tile_v1"]->setFloat(-.15f, 0, 0.25f);
	floorMaterial["crack_color"]->setFloat(0.1f, 0.1f, 0.1f);
	floorMaterial["crack_width"]->setFloat(0.02f);

	materials[FLOOR] = floorMaterial;

	Material wallMaterial = ctx->createMaterial();
	wallMaterial->setClosestHitProgram(0, phongClosestHit);
	wallMaterial->setAnyHitProgram(1, shadowAnyHit);

	wallMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	wallMaterial["Kd"]->setFloat(0.6f, 0.7f, 0.8f);
	wallMaterial["Ks"]->setFloat(0.8f, 0.9f, 0.8f);
	wallMaterial["phong_exp"]->setFloat(88.0f);

	materials[WALL] = wallMaterial;

	Material mirrorMaterial = ctx->createMaterial();
	mirrorMaterial->setClosestHitProgram(0, mirrorClosestHit);
	mirrorMaterial->setAnyHitProgram(1, shadowAnyHit);

	mirrorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	mirrorMaterial["Kd"]->setFloat(0.6f, 0.7f, 0.8f);
	mirrorMaterial["Ks"]->setFloat(0.8f, 0.9f, 0.8f);
	mirrorMaterial["phong_exp"]->setFloat(88.0f);
    mirrorMaterial["reflectivity"]->setFloat( 0.1f, 0.1f, 0.1f );

	materials[MIRROR] = mirrorMaterial;

	Material glassMaterial = ctx->createMaterial();
	glassMaterial->setClosestHitProgram(0, glassClosestHit);
	glassMaterial->setAnyHitProgram(1, glassAnyHit);

	glassMaterial["importance_cutoff"]->setFloat(1e-2f);
    glassMaterial["cutoff_color"]->setFloat(0.55f, 0.55f, 0.55f);
    glassMaterial["fresnel_exponent"]->setFloat(3.0f);
    glassMaterial["fresnel_minimum"]->setFloat(0.1f);
    glassMaterial["fresnel_maximum"]->setFloat(1.0f);
    glassMaterial["refraction_index"]->setFloat(1.4f);
    glassMaterial["refraction_color"]->setFloat(1.0f, 1.0f, 1.0f);
    glassMaterial["reflection_color"]->setFloat(1.0f, 1.0f, 1.0f);
    float3 extinction = make_float3(.80f, .80f, .80f);
    glassMaterial["extinction_constant"]->setFloat(log(extinction.x), log(extinction.y), log(extinction.z));
    glassMaterial["shadow_attenuation"]->setFloat(0.4f, 0.4f, 0.4f);

	materials[GLASS] = glassMaterial;
}

void Scene::createSceneGraph(const Labyrinth &lab)
{
	std::string pathBox = pathToPTX("box.cu");
	Program boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");
	Program boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");

	vector<GeometryInstance> gis;
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
		gis.push_back(ctx->createGeometryInstance(box, &materials[walls[i].matIdx], &materials[walls[i].matIdx]+1));
	}

	std::string pathFloor = pathToPTX("rectangleAA.cu");
	Geometry floor = ctx->createGeometry();
	floor->setPrimitiveCount(1);
	floor->setBoundingBoxProgram(ctx->createProgramFromPTXFile(pathFloor, "bounds"));
	floor->setIntersectionProgram(ctx->createProgramFromPTXFile(pathFloor, "intersect"));

	float rw = lab.getRealWidth(), rh = lab.getRealHeight();
	floor["plane_normal"]->setFloat(0.0f, 1.0f, 0.0f);
	floor["recmin"]->setFloat(-rw / 2.0f, 0.0f, -rh / 2.0f);
	floor["recmax"]->setFloat(rw / 2.0f, 0.0f, rh / 2.0f);

	gis.push_back(ctx->createGeometryInstance(floor, &materials[FLOOR], &materials[FLOOR]+1));

	// Place all in group
	GeometryGroup geometrygroup = ctx->createGeometryGroup();
	geometrygroup->setChildCount(gis.size());
	for(int i = 0; i < gis.size(); ++i)
		geometrygroup->setChild(i, gis[i]);
	geometrygroup->setAcceleration(ctx->createAcceleration("Sbvh", "Bvh"));
	//geometrygroup->setAcceleration(ctx->createAcceleration("NoAccel", "NoAccel"));

	ctx["top_object"]->set(geometrygroup);

	ctx->validate();
	//ctx->compile();
}

void Scene::trace()
{
	ctx->launch(0, Settings::GS["bufferWidth"], Settings::GS["bufferHeight"]);
}

void Scene::setCamera(const Camera &cam)
{
	float tanfov = tanf(cam.FoV * pi / 360.0f) * 0.5f;
	vec3 eye = cam.position;
	vec3 U = cam.getRight() * tanfov * cam.aspectRatio;
	vec3 V = cam.getUp() * tanfov;
	vec3 W = cam.getDirection();

	ctx["eye"]->setFloat(make_float3(eye.x, eye.y, eye.z));
	ctx["U"]->setFloat(make_float3(U.x, U.y, U.z));
	ctx["V"]->setFloat(make_float3(V.x, V.y, V.z));
	ctx["W"]->setFloat(make_float3(W.x, W.y, W.z));
}
