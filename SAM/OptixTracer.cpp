#include "OptixTracer.h"
#include "Utils.h"
#include "Environment.h"
#include "Programs.h"
#include <TextureHandler.h>

using namespace optix;
using namespace reng;
using namespace utils;
using namespace glm;
using namespace std;

OptixTracer::OptixTracer(void) :
	SETTING(useInternalReflections), SETTING(castsShadows),
	SETTING(useSchlick), SETTING(maxRayDepth), matHandler(ctx)
{
}


OptixTracer::~OptixTracer(void)
{
	//ctx->destroy();
}

Buffer OptixTracer::getBuffer()
{
	return ctx["output_buffer"]->getBuffer();
}

void OptixTracer::setBufferSize(int w, int h)
{
	w = max(1, w);
	w = min(w, Environment::get().maxBufferWidth);

	h = max(1, h);
	h = min(h, Environment::get().maxBufferHeight);
	ctx["output_buffer"]->getBuffer()->setSize(w, h);
}

void OptixTracer::initialize(unsigned int GLBO)
{
	ctx = Context::create();

	ctx->setRayTypeCount(2);
	ctx->setEntryPointCount(1);
	ctx->setStackSize(2048);

	ctx["radiance_ray_type"]->setUint(0);
	ctx["shadow_ray_type"]->setUint(1);
	ctx["scene_epsilon"]->setFloat(1.e-2f);
	ctx["importance_cutoff"]->setFloat(0.01f);
	ctx["ambient_light_color"]->setFloat(0.3f, 0.3f, 0.3f);

	ctx["max_depth"]->setInt(maxRayDepth);
	ctx["cast_shadows"]->setInt(castsShadows);
	ctx["use_schlick"]->setInt(useSchlick);
	ctx["use_internal_reflections"]->setInt(useInternalReflections);

	Buffer buff = ctx->createBufferFromGLBO(RT_BUFFER_OUTPUT, GLBO);
	buff->setFormat(RT_FORMAT_FLOAT4);
	buff->setSize(Environment::get().bufferWidth, Environment::get().bufferHeight);
	ctx["output_buffer"]->setBuffer(buff);

	Buffer lightBuffer = ctx->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	lightBuffer->setFormat(RT_FORMAT_USER);
	lightBuffer->setElementSize(sizeof(BasicLight));
	ctx["lights"]->setBuffer(lightBuffer);

	Programs::init(ctx);

	ctx->setRayGenerationProgram(0, Programs::rayGeneration);

	ctx->setExceptionProgram(0, Programs::exception);
	ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

	ctx->setMissProgram(0, Programs::envmapMiss);
	Texture tex = TextureHandler::getTexture(utils::defTexture("environment.jpg"));
	optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(tex.getID(), RT_TARGET_GL_TEXTURE_2D);
	sampler->setWrapMode(0, RT_WRAP_REPEAT);
	sampler->setWrapMode(1, RT_WRAP_REPEAT);
	sampler->setWrapMode(2, RT_WRAP_REPEAT);
	sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
	sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
	sampler->setMaxAnisotropy(1.0f);
	sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
	ctx["envmap"]->setTextureSampler(sampler);

	matHandler.createLabMaterials();
}

void OptixTracer::addMesh(const Labyrinth &lab)
{
	const vector<Box> &walls = lab.getWalls();
	int n = walls.size();
	for(int i = 0; i < n; ++i)
	{
		Geometry box = ctx->createGeometry();
		box->setPrimitiveCount(1);
		box->setBoundingBoxProgram(Programs::boxAABB);
		box->setIntersectionProgram(Programs::boxIntersect);
		box["boxmin"]->setFloat(walls[i].boxmin);
		box["boxmax"]->setFloat(walls[i].boxmax);
		gis.push_back(ctx->createGeometryInstance(box, &matHandler.getLabyrinthMaterial(walls[i].matIdx), 
			&matHandler.getLabyrinthMaterial(walls[i].matIdx)+1));
	}

	std::string pathFloor = pathToPTX("rectangleAA.cu"); //TODO: texture floor
	Geometry floor = ctx->createGeometry();
	floor->setPrimitiveCount(1);
	floor->setBoundingBoxProgram(ctx->createProgramFromPTXFile(pathFloor, "bounds"));
	floor->setIntersectionProgram(ctx->createProgramFromPTXFile(pathFloor, "intersect"));

	float rw = lab.getRealWidth(), rh = lab.getRealHeight();
	floor["plane_normal"]->setFloat(0.0f, 1.0f, 0.0f);
	floor["recmin"]->setFloat(-rw / 2.0f, 0.0f, -rh / 2.0f);
	floor["recmax"]->setFloat(rw / 2.0f, 0.0f, rh / 2.0f);

	gis.push_back(ctx->createGeometryInstance(floor, &matHandler.getLabyrinthMaterial(MaterialHandler::LabMaterials::WALL), &matHandler.getLabyrinthMaterial(MaterialHandler::LabMaterials::WALL)+1));
}

template<class T>
Buffer OptixTracer::getBufferFromVector(const vector<T> &vec, RTformat type)
{
	Buffer ret = ctx->createBuffer(RT_BUFFER_INPUT);
	ret->setFormat(type);
	ret->setSize(vec.size());
	memcpy(static_cast<void*>(ret->map()), (const void*)vec.data(), vec.size() * sizeof(T));
	ret->unmap();
	return ret;
}

void OptixTracer::addMesh(const string &path, const aiMesh *mesh, const aiMaterial *mat)
{
	vector<int3> indices;
	for(int i = 0; i < mesh->mNumFaces; ++i)
		indices.push_back(make_int3(mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]));

	vector<float3> vertexData;
	vector<float3> normalData;
	vector<float2> uvData;

	for(int i = 0; i < mesh->mNumVertices; ++i)
	{
		vertexData.push_back(make_float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		normalData.push_back(make_float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		if(mesh->HasTextureCoords(0))
			uvData.push_back(make_float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
	}
	if(uvData.empty())
		uvData = vector<float2>(mesh->mNumVertices, make_float2(0.0f));

	Geometry gMesh = ctx->createGeometry();
	gMesh->setPrimitiveCount(indices.size());
	gMesh->setBoundingBoxProgram(Programs::meshBoundingBox);
	gMesh->setIntersectionProgram(Programs::meshIntersect);

	gMesh["vertex_buffer"]->setBuffer(getBufferFromVector(vertexData, RT_FORMAT_FLOAT3));
	gMesh["normal_buffer"]->setBuffer(getBufferFromVector(normalData, RT_FORMAT_FLOAT3));
	gMesh["texcoord_buffer"]->setBuffer(getBufferFromVector(uvData, RT_FORMAT_FLOAT2));
	gMesh["index_buffer"]->setBuffer(getBufferFromVector(indices, RT_FORMAT_INT3));

	Material material = matHandler.createMaterial(path, mat);
	
	GeometryInstance inst = ctx->createGeometryInstance();
	inst->setMaterialCount(1);
	inst->setGeometry(gMesh);
	inst->setMaterial(0, material);

	gis.push_back(inst);
}

void OptixTracer::addMesh(int mat, const aiMesh *mesh)
{
	vector<int3> indices;
	for(int i = 0; i < mesh->mNumFaces; ++i)
		indices.push_back(make_int3(mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]));

	vector<float3> vertexData;
	vector<float3> normalData;
	vector<float2> uvData;

	for(int i = 0; i < mesh->mNumVertices; ++i)
	{
		vertexData.push_back(make_float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		normalData.push_back(make_float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		if(mesh->HasTextureCoords(0))
			uvData.push_back(make_float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
	}
	if(uvData.empty())
		uvData = vector<float2>(mesh->mNumVertices, make_float2(0.0f));

	Geometry gMesh = ctx->createGeometry();
	gMesh->setPrimitiveCount(indices.size());
	gMesh->setBoundingBoxProgram(Programs::meshBoundingBox);
	gMesh->setIntersectionProgram(Programs::meshIntersect);

	gMesh["vertex_buffer"]->setBuffer(getBufferFromVector(vertexData, RT_FORMAT_FLOAT3));
	gMesh["normal_buffer"]->setBuffer(getBufferFromVector(normalData, RT_FORMAT_FLOAT3));
	gMesh["texcoord_buffer"]->setBuffer(getBufferFromVector(uvData, RT_FORMAT_FLOAT2));
	gMesh["index_buffer"]->setBuffer(getBufferFromVector(indices, RT_FORMAT_INT3));

	GeometryInstance inst = ctx->createGeometryInstance();
	inst->setMaterialCount(1);
	inst->setGeometry(gMesh);
	inst->setMaterial(0, matHandler.getLabyrinthMaterial(mat));

	gis.push_back(inst);
}

void OptixTracer::addScene(const std::string &path, const aiScene *scene)
{
	for(int i = 0; i < scene->mNumMeshes; ++i)
		addMesh(path, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
}

void OptixTracer::addScene(int mat, const aiScene * scene)
{
	for(int i = 0; i < scene->mNumMeshes; ++i)
		addMesh(mat, scene->mMeshes[i]);
}

void OptixTracer::addLight(const BasicLight &light)
{
	lights.push_back(light);
}

void OptixTracer::compileSceneGraph()
{
	ctx["lights"]->getBuffer()->setSize(lights.size());
	memcpy(ctx["lights"]->getBuffer()->map(), (const void*)lights.data(), lights.size() * sizeof(BasicLight));
	ctx["lights"]->getBuffer()->unmap();

	GeometryGroup geometrygroup = ctx->createGeometryGroup();
	geometrygroup->setChildCount(gis.size());
	for(int i = 0; i < gis.size(); ++i)
		geometrygroup->setChild(i, gis[i]);

	Acceleration accel = ctx->createAcceleration("Sbvh", "Bvh");

	accel->setProperty("index_buffer_name", "index_buffer");
	accel->setProperty("vertex_buffer_name", "vertex_buffer");
	geometrygroup->setAcceleration(accel);
	ctx["top_object"]->set(geometrygroup);
	ctx->validate();
	ctx->compile();
}


void OptixTracer::clearSceneGraph()
{
	gis.clear();
}

void OptixTracer::trace()
{
	ctx->launch(0, Environment::get().bufferWidth, Environment::get().bufferHeight);
}

BasicLight& OptixTracer::getLight(int i)
{
	return lights[i];
}

void OptixTracer::updateLight(int idx)
{
	memcpy((void*)(((BasicLight*)ctx["lights"]->getBuffer()->map())+idx), (BasicLight*)lights.data()+idx, sizeof(BasicLight));
	ctx["lights"]->getBuffer()->unmap();
}

void OptixTracer::setCamera(const Camera &cam)
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
