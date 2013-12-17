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
	ctx["scene_epsilon"]->setFloat(1.e-3f);

	Buffer buff = ctx->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
	ctx["output_buffer"]->setBuffer(buff);

	std::string path = pathToPTX("shaders.cu");

	ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(path, "pinhole_camera"));

	ctx->setExceptionProgram(0, ctx->createProgramFromPTXFile(path, "exception"));
	ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

	ctx->setMissProgram(0, ctx->createProgramFromPTXFile(path, "miss"));
	ctx["miss_color"]->setFloat(0.0f, 1.0f, 1.0f);

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

	createSceneGraph();

	ctx->validate();
	ctx->compile();
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
	box["boxmax"]->setFloat(2.0f, 7.0f,  2.0f);

	Geometry box2 = ctx->createGeometry();
	box2->setPrimitiveCount(1);
	box2->setBoundingBoxProgram(boxAABB);
	box2->setIntersectionProgram(boxIntersect);
	box2["boxmin"]->setFloat(6.0f, 0.0f, -2.0f);
	box2["boxmax"]->setFloat(8.0f, 7.0f,  2.0f);

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

	std::string mainPath(pathToPTX("shaders.cu"));
	Material floorMaterial = ctx->createMaterial();
	Program floorClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_radiance_floor");
	floorMaterial->setClosestHitProgram(0, floorClosestHit);
	
	
	floorMaterial["ambient_light_color"]->setFloat(0.5f, 0.5f, 0.5f);
	floorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	floorMaterial["Kd"]->setFloat(0.6f, 0.7f, 0.8f);
	floorMaterial["Ks"]->setFloat(0.8f, 0.9f, 0.8f);
	floorMaterial["phong_exp"]->setFloat(88);

	std::vector<GeometryInstance> gis;
	gis.push_back(ctx->createGeometryInstance(box, &floorMaterial, &floorMaterial+1));
	gis.push_back(ctx->createGeometryInstance(box2, &floorMaterial, &floorMaterial+1));
	gis.push_back(ctx->createGeometryInstance(parallelogram, &floorMaterial, &floorMaterial+1));

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