#include "Scene.h"
#include "Utils.h"

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

void Scene::initialize()
{
	ctx = Context::create();

	ctx->setRayTypeCount(2);
	ctx->setEntryPointCount(1);
	ctx->setStackSize(4640);


	ctx["radiance_ray_type"]->setUint(0);
	ctx["scene_epsilon"]->setFloat( 1.e-3f );

	Buffer buff = ctx->createBuffer(RT_BUFFER_OUTPUT, RT_FORMAT_UNSIGNED_BYTE4, width, height);
	ctx["output_buffer"]->setBuffer(buff);

	std::string path = pathToPTX("shaders.cu");
	ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(path, "pinhole_camera"));

	ctx->setExceptionProgram(0, ctx->createProgramFromPTXFile(path, "exception"));
	ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

	ctx->setMissProgram(0, ctx->createProgramFromPTXFile(path, "miss"));
	ctx["miss_color"]->setFloat(0.0f, 1.0f, 1.0f);

	ctx["eye"]->setFloat(make_float3(7.0f, 9.2f, -6.0f));
	ctx["U"]->setFloat(make_float3(-6.0f, 0.0f, -7.0f));
	ctx["V"]->setFloat(make_float3(-2.3f, 5.3f, 2.0f));
	ctx["W"]->setFloat(make_float3(-7.0f, -5.2f, 6.0f));

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
	Material box_matl = ctx->createMaterial();
	Program box_ch = ctx->createProgramFromPTXFile(mainPath, "closest_hit_radiance");
	box_matl->setClosestHitProgram(0, box_ch);

	Material floor_matl = ctx->createMaterial();
	Program floor_ch = ctx->createProgramFromPTXFile(mainPath, "closest_hit_radiance");
	floor_matl->setClosestHitProgram(0, floor_ch);

	std::vector<GeometryInstance> gis;
	gis.push_back(ctx->createGeometryInstance(box, &box_matl, &box_matl+1));
	gis.push_back(ctx->createGeometryInstance( parallelogram, &floor_matl, &floor_matl+1 ));

	// Place all in group
	GeometryGroup geometrygroup = ctx->createGeometryGroup();
	geometrygroup->setChildCount(gis.size());
	geometrygroup->setChild(0, gis[0]);
	geometrygroup->setChild(1, gis[1]);
	geometrygroup->setAcceleration(ctx->createAcceleration("NoAccel","NoAccel"));

	ctx["top_object"]->set(geometrygroup);
}

void Scene::trace()
{
	ctx->launch(0, width, height);
}

void Scene::setCamera(const vec3 &eye, const vec3 &dir, const vec3 &up, const vec3 &right)
{
	ctx["eye"]->setFloat(make_float3(eye.x, eye.y, eye.z));
	ctx["U"]->setFloat(make_float3(right.x, right.y, right.z));
	ctx["V"]->setFloat(make_float3(up.x, up.y, up.z));
	ctx["W"]->setFloat(make_float3(dir.x, dir.y, dir.z));
}