#include "Scene.h"
#include "Utils.h"
#include "lights.h"

using namespace optix;
using namespace utils;
using namespace glm;

Scene::Scene(int width, int height)
	: width(width), height(height)
{
}


Scene::~Scene(void)
{
	ctx->destroy();
}


Buffer Scene::getBuffer()
{
	return ctx["output_buffer"]->getBuffer();
}


unsigned int Scene::getWidth() const
{
	return width;
}

unsigned int Scene::getHeight() const
{
	return height;
}

void Scene::setBufferSize(int w, int h)
{
	width = max(1, w);
	height = max(1, h);
	ctx["output_buffer"]->getBuffer()->setSize(width, height);
}

void Scene::initialize()
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

	Buffer buff = ctx->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
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
		{make_float3( -5.0f, 60.0f, -16.0f ), make_float3( 1.0f, 1.0f, 1.0f ), 1}
	};

	Buffer lightBuffer = ctx->createBuffer(RT_BUFFER_INPUT);
	lightBuffer->setFormat(RT_FORMAT_USER);
	lightBuffer->setElementSize(sizeof(BasicLight));
	lightBuffer->setSize(sizeof(lights) / sizeof(lights[0]));
	memcpy(lightBuffer->map(), lights, sizeof(lights));
	lightBuffer->unmap();

	ctx["lights"]->set(lightBuffer);

	createMaterials();
	createSceneGraph();

	ctx->validate();
	ctx->compile();
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

void Scene::createSceneGraph()
{
	std::string pathBox = pathToPTX("box.cu");
	Program boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");
	Program boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");

	Geometry box = ctx->createGeometry();
	box->setPrimitiveCount(1);
	box->setBoundingBoxProgram(boxAABB);
	box->setIntersectionProgram(boxIntersect);
	box["boxmin"]->setFloat(-2.0f, 0.0f, -2.0f);
	box["boxmax"]->setFloat(2.0f, 7.0f, 2.0f);

	Geometry box2 = ctx->createGeometry();
	box2->setPrimitiveCount(1);
	box2->setBoundingBoxProgram(boxAABB);
	box2->setIntersectionProgram(boxIntersect);
	box2["boxmin"]->setFloat(6.0f, 0.0f, -2.0f);
	box2["boxmax"]->setFloat(8.0f, 7.0f, 2.0f);

	Geometry box3 = ctx->createGeometry();
	box3->setPrimitiveCount(1);
	box3->setBoundingBoxProgram(boxAABB);
	box3->setIntersectionProgram(boxIntersect);
	box3["boxmin"]->setFloat(6.0f, 0.0f, 3.0f);
	box3["boxmax"]->setFloat(8.0f, 7.0f, 7.0f);

	std::string pathFloor = pathToPTX("parallelogram.cu");
	Geometry parallelogram = ctx->createGeometry();
	parallelogram->setPrimitiveCount(1);
	parallelogram->setBoundingBoxProgram(ctx->createProgramFromPTXFile(pathFloor, "bounds"));
	parallelogram->setIntersectionProgram(ctx->createProgramFromPTXFile(pathFloor, "intersect"));

	float3 anchor = make_float3(-64.0f, 0.01f, -64.0f);
	float3 v1 = make_float3(128.0f, 0.0f, 0.0f);
	float3 v2 = make_float3(0.0f, 0.0f, 128.0f);
	float3 normal = cross(v2, v1);
	normal = normalize(normal);
	float d = dot(normal, anchor);
	v1 *= 1.0f / dot(v1, v1);
	v2 *= 1.0f / dot(v2, v2);
	float4 plane = make_float4(normal, d);
	parallelogram["plane"]->setFloat(plane);
	parallelogram["v1"]->setFloat(v1);
	parallelogram["v2"]->setFloat(v2);
	parallelogram["anchor"]->setFloat(anchor);

	std::vector<GeometryInstance> gis;
	gis.push_back(ctx->createGeometryInstance(box, &materials[WALL], &materials[WALL]+1));
	gis.push_back(ctx->createGeometryInstance(box2, &materials[MIRROR], &materials[MIRROR]+1));
	gis.push_back(ctx->createGeometryInstance(box3, &materials[GLASS], &materials[GLASS]+1));
	gis.push_back(ctx->createGeometryInstance(parallelogram, &materials[FLOOR], &materials[FLOOR]+1));

	// Place all in group
	GeometryGroup geometrygroup = ctx->createGeometryGroup();
	geometrygroup->setChildCount(gis.size());
	for(int i = 0; i < gis.size(); ++i)
		geometrygroup->setChild(i, gis[i]);
	geometrygroup->setAcceleration(ctx->createAcceleration("NoAccel","NoAccel"));

	ctx["top_object"]->set(geometrygroup);
}

void Scene::trace()
{
	ctx->launch(0, width, height);
}

void Scene::setCamera(const Camera &cam)
{
	float tanfov = tanf(cam.FoV * 3.14f / 360.0f) * 2.0f;
	vec3 eye = cam.position;
	vec3 U = cam.getRight() * tanfov * cam.aspectRatio;
	vec3 V = cam.getUp() * tanfov;
	vec3 W = cam.getDirection();

	ctx["eye"]->setFloat(make_float3(eye.x, eye.y, eye.z));
	ctx["U"]->setFloat(make_float3(U.x, U.y, U.z));
	ctx["V"]->setFloat(make_float3(V.x, V.y, V.z));
	ctx["W"]->setFloat(make_float3(W.x, W.y, W.z));
}