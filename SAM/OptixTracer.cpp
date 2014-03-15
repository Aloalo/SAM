#include "OptixTracer.h"
#include "Utils.h"
#include "Environment.h"
#include "Programs.h"
#include "OptixTextureHandler.h"
#include <fstream>

using namespace optix;
using namespace reng;
using namespace glm;
using namespace std;

namespace trayc
{
	OptixTracer::OptixTracer(void) :
		SETTING(useInternalReflections),
		SETTING(castsShadows),
		SETTING(useSchlick),
		SETTING(maxRayDepth),
		SETTING(MSAA),
		SETTING(renderingDivisionLevel)
	{
	}

	OptixTracer::~OptixTracer(void)
	{
	}

	Buffer OptixTracer::getBuffer()
	{
		return Environment::get().ctx["output_buffer"]->getBuffer();
	}

	void OptixTracer::setBufferSize(int w, int h)
	{
		w = max(1, w);
		h = max(1, h);
		ctx["output_buffer"]->getBuffer()->setSize(w, h);
	}

	void OptixTracer::initialize(unsigned int GLBO)
	{
		ctx = Context::create();
		printf("Available device memory: %d MB\n", ctx->getAvailableDeviceMemory(0) >> 20);

		Programs::init(ctx);

		ctx->setRayTypeCount(2);
		ctx->setEntryPointCount(1);
		ctx->setCPUNumThreads(4);
		ctx->setStackSize(768 + 256 * maxRayDepth);

		ctx["radiance_ray_type"]->setUint(0);
		ctx["shadow_ray_type"]->setUint(1);
		ctx["scene_epsilon"]->setFloat(1.e-2f);
		ctx["importance_cutoff"]->setFloat(0.01f);
		ctx["renderingDivisionLevel"]->setInt(renderingDivisionLevel);
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

		if(MSAA > 1)
		{
			ctx->setRayGenerationProgram(0, Programs::rayGenerationAA);
			ctx["AAlevel"]->setInt(MSAA);
		}
		else
			ctx->setRayGenerationProgram(0, Programs::rayGeneration);

		ctx->setMissProgram(0, Programs::envmapMiss);
		ctx["envmap"]->setTextureSampler(OptixTextureHandler::get().get(Utils::defTexture("environment.jpg")));

		ctx->setExceptionProgram(0, Programs::exception);
		ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);
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

	Geometry OptixTracer::getGeometry(const aiMesh *mesh, const aiMaterial *mat, const std::string &path)
	{
		vector<int3> indices;
		indices.reserve(mesh->mNumFaces);
		for(int i = 0; i < mesh->mNumFaces; ++i)
			indices.push_back(make_int3(mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]));

		vector<float3> vertexData;
		vertexData.reserve(mesh->mNumVertices);
		vector<float3> normalData;
		normalData.reserve(mesh->mNumVertices);
		vector<float3> tangentData;
		tangentData.reserve(mesh->mNumVertices);
		vector<float2> uvData;
		uvData.reserve(mesh->mNumVertices);

		bool hasNormalMap = true;//mat == NULL ? 0 : mat->GetTextureCount(aiTextureType_HEIGHT) || mat->GetTextureCount(aiTextureType_NORMALS);

		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			vertexData.push_back(Utils::aiToOptix(mesh->mVertices[i]));
			normalData.push_back(Utils::aiToOptix(mesh->mNormals[i]));
			if(hasNormalMap)
				tangentData.push_back(Utils::aiToOptix(mesh->mTangents[i]));

			if(mesh->HasTextureCoords(0))
				uvData.push_back(make_float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
		}

		Geometry gMesh = ctx->createGeometry();
		gMesh->setPrimitiveCount(indices.size());
		gMesh->setBoundingBoxProgram(Programs::meshBoundingBox);
		if(hasNormalMap)
			gMesh->setIntersectionProgram(Programs::meshIntersectNormalMap);
		else
			gMesh->setIntersectionProgram(Programs::meshIntersectNoNormalMap);

		gMesh["vertex_buffer"]->setBuffer(getBufferFromVector(vertexData, RT_FORMAT_FLOAT3));
		gMesh["normal_buffer"]->setBuffer(getBufferFromVector(normalData, RT_FORMAT_FLOAT3));
		if(hasNormalMap)
		{
			gMesh["tangent_buffer"]->setBuffer(getBufferFromVector(tangentData, RT_FORMAT_FLOAT3));
			gMesh["normal_map"]->setTextureSampler(OptixTextureHandler::get().get(MaterialHandler::get().getTextureName(mat, aiTextureType_HEIGHT, path), 
				Utils::defTexture("bumpDefault.png"), 0.0f, RT_WRAP_REPEAT));
		}
		gMesh["texcoord_buffer"]->setBuffer(getBufferFromVector(uvData, RT_FORMAT_FLOAT2));
		gMesh["index_buffer"]->setBuffer(getBufferFromVector(indices, RT_FORMAT_INT3));

		return gMesh;
	}

	void OptixTracer::addMesh(const string &path, const aiMesh *mesh, const aiMaterial *mat)
	{
		Geometry gMesh = getGeometry(mesh, mat, path);
		Material material = MaterialHandler::get().createMaterial(path, mat);

		GeometryInstance inst = ctx->createGeometryInstance();
		inst->setMaterialCount(1);
		inst->setGeometry(gMesh);
		inst->setMaterial(0, material);

		gis.push_back(inst);
	}

	void OptixTracer::addMesh(const optix::Material &mat, const aiMesh *mesh)
	{
		Geometry gMesh = getGeometry(mesh);

		GeometryInstance inst = ctx->createGeometryInstance();
		inst->setMaterialCount(1);
		inst->setGeometry(gMesh);
		inst->setMaterial(0, mat);

		gis.push_back(inst);
	}

	void OptixTracer::addScene(const std::string &path, const aiScene *scene)
	{
		for(int i = 0; i < scene->mNumMeshes; ++i)
			addMesh(path, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]);
	}

	void OptixTracer::addScene(const optix::Material &mat, const aiScene * scene)
	{
		for(int i = 0; i < scene->mNumMeshes; ++i)
			addMesh(mat, scene->mMeshes[i]);
	}

	void OptixTracer::addLight(const BasicLight &light)
	{
		lights.push_back(light);
	}

	void OptixTracer::addGeometryInstance(const GeometryInstance &gi)
	{
		gis.push_back(gi);
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

		Transform trans = ctx->createTransform();
		trans->setChild(geometrygroup);
		float s = 0.05f;
		trans->setMatrix(false, (float*)&scale(mat4(1.0f), vec3(s)), (float*)&inverse(scale(mat4(1.0f), vec3(s))));
		ctx["top_object"]->set(trans);

		geometrygroup->setAcceleration(ctx->createAcceleration("Sbvh", "Bvh"));

		accelHandler.setMesh(Utils::resource("accelCaches/accel.accelcache"));
		accelHandler.loadAccelCache(geometrygroup);

		if(!accelHandler.accel_cache_loaded)
		{
			Acceleration accel = ctx->createAcceleration("Sbvh", "Bvh");

			accel->setProperty("index_buffer_name", "index_buffer");
			accel->setProperty("vertex_buffer_name", "vertex_buffer");
			accel->markDirty();
			geometrygroup->setAcceleration(accel);
			ctx->launch(0, 0, 0);

			accelHandler.saveAccelCache(geometrygroup);
		}
		ctx->validate();
		ctx->compile();

		printf("Available device memory after compile: %d MB\n", ctx->getAvailableDeviceMemory(0) >> 20);
	}


	void OptixTracer::clearSceneGraph()
	{
		gis.clear();
	}

	void OptixTracer::trace()
	{
		for(int i = 0; i < renderingDivisionLevel; ++i)
		{
			ctx["myStripe"]->setInt(i);
			ctx->launch(0, Environment::get().bufferWidth, Environment::get().bufferHeight / renderingDivisionLevel);
		}
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
		float tanfov = tanf(cam.FoV * Utils::pi / 360.0f) * 0.5f;

		ctx["eye"]->setFloat(Utils::glmToOptix(cam.position));
		ctx["U"]->setFloat(Utils::glmToOptix(cam.getRight() * tanfov * cam.aspectRatio));
		ctx["V"]->setFloat(Utils::glmToOptix(cam.getUp() * tanfov));
		ctx["W"]->setFloat(Utils::glmToOptix(cam.getDirection()));
	}

	void OptixTracer::renderToPPM(const std::string &name)
	{
		trace();

		Buffer buff = getBuffer();
		RTsize w, h;
		buff->getSize(w, h);
		int k = 4;

		float *out = (float*)buff->map();
		{
			std::ofstream ofs(name, std::ios::out | std::ios::binary);
			ofs << "P6\n" << w << " " << h << "\n255\n";
			for(int i = h-1; i >= 0; --i)
				for(int j = 0; j < w; ++j)
				{
					ofs << (unsigned char)(std::min(1.0f, out[(i*w+j)*k + 0]) * 255) << 
						   (unsigned char)(std::min(1.0f, out[(i*w+j)*k + 1]) * 255) <<
						   (unsigned char)(std::min(1.0f, out[(i*w+j)*k + 2]) * 255); 
				}
				ofs.close();
		}
		buff->unmap();
	}
}