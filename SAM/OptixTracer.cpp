#include "OptixTracer.h"
#include "Utils.h"
#include "Environment.h"
#include <TextureHandler.h>

using namespace optix;
using namespace reng;
using namespace utils;
using namespace glm;
using namespace std;

OptixTracer::OptixTracer(void) :
	SETTING(useInternalReflections), SETTING(castsShadows),
	SETTING(useSchlick), SETTING(maxRayDepth)
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
	ctx["scene_epsilon"]->setFloat(1.e-3f);
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

	std::string path = pathToPTX("shaders.cu");

	ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(path, "pinhole_camera"));

	ctx->setExceptionProgram(0, ctx->createProgramFromPTXFile(path, "exception"));
	ctx["bad_color"]->setFloat(1.0f, 0.0f, 0.0f);

	/*ctx->setMissProgram(0, ctx->createProgramFromPTXFile(path, "gradient_miss"));
	ctx["miss_min"]->setFloat(0.3f, 0.3f, 0.3f);
	ctx["miss_max"]->setFloat(0.8f, 0.8f, 0.8f);*/

	ctx->setMissProgram(0, ctx->createProgramFromPTXFile(path, "envmap_miss"));
	Texture tex = TextureHandler::getTexture(utils::texPath, std::string("environment.png"));
	optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(tex.getID(), RT_TARGET_GL_TEXTURE_2D);
	sampler->setWrapMode(0, RT_WRAP_REPEAT);
	sampler->setWrapMode(1, RT_WRAP_REPEAT);
	sampler->setWrapMode(2, RT_WRAP_REPEAT);
	sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
	sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
	sampler->setMaxAnisotropy(1.0f);
	sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
	ctx["envmap"]->setTextureSampler(sampler);


	vector<BasicLight> lights;
	
	lights.push_back(BasicLight( //light0 - point light
			make_float3(-5.0f, 180.0f, -16.0f), //pos/dir
			make_float3(1.0f, 1.0f, 1.0f), //color
			make_float3(1.0f, 0.0f, 0.0f), //attenuation
			make_float3(1.0f, 0.0f, 0.0f), //spot_direction
			360.0f, //spot_cutoff
			0.0f, //spot_exponent
			1, //casts_shadows
			0 //is_directional
		));

	Buffer lightBuffer = ctx->createBuffer(RT_BUFFER_INPUT_OUTPUT);
	lightBuffer->setFormat(RT_FORMAT_USER);
	lightBuffer->setElementSize(sizeof(BasicLight));
	lightBuffer->setSize(lights.size());
	memcpy(lightBuffer->map(), (const void*)lights.data(), lights.size() * sizeof(BasicLight));
	lightBuffer->unmap();

	ctx["lights"]->setBuffer(lightBuffer);

	createMaterials();
}

void OptixTracer::createMaterials()
{
	std::string mainPath(pathToPTX("shaders.cu"));
	Program phongClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_phong");
	Program shadowAnyHit = ctx->createProgramFromPTXFile(mainPath, "any_hit_solid");
	Program glassAnyHit = ctx->createProgramFromPTXFile(mainPath, "any_hit_shadow_glass");
	Program glassClosestHit = ctx->createProgramFromPTXFile(mainPath, "closest_hit_glass");

	Material wallMaterial = ctx->createMaterial();
	wallMaterial->setClosestHitProgram(0, phongClosestHit);
	wallMaterial->setAnyHitProgram(1, shadowAnyHit);

	wallMaterial["Ka"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["Kd"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["phong_exp"]->setFloat(88.0f);

	materials[WALL] = wallMaterial;
	materials[FLOOR] = wallMaterial;


	Material mirrorMaterial = ctx->createMaterial();
	mirrorMaterial->setClosestHitProgram(0, phongClosestHit);
	mirrorMaterial->setAnyHitProgram(1, shadowAnyHit);

	mirrorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	mirrorMaterial["Kd"]->setFloat(0.7f, 0.7f, 0.7f);
	mirrorMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
	mirrorMaterial["phong_exp"]->setFloat(88.0f);
	mirrorMaterial["reflectivity"]->setFloat(0.7f, 0.7f, 0.7f);

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

void OptixTracer::createSceneGraph(const Labyrinth &lab)
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

optix::TextureSampler OptixTracer::getTexture(const aiMaterial *mat, aiTextureType type)
{
	Texture ret;
	string name;
	if(mat->GetTextureCount(type) > 0)
	{
		aiString name;
		if(mat->GetTexture(type, 0, &name, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			ret = TextureHandler::getTexture(utils::texPath, name.C_Str());
	}
	else
	{
		bool ok = !TextureHandler::hasTexture("emptyTex");
		ret = TextureHandler::getTexture("emptyTex", GL_TEXTURE_2D);
		if(ok)
		{
			float data[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			ret.bind();
			ret.texImage(0, GL_RGBA32F_ARB, vec3(1, 1, 0), GL_RGBA, GL_FLOAT, data);
		}
	}

	if(textures.find(ret.getID()) == textures.end())
	{
		optix::TextureSampler sampler = ctx->createTextureSamplerFromGLImage(ret.getID(), RT_TARGET_GL_TEXTURE_2D);
		sampler->setWrapMode(0, RT_WRAP_REPEAT);
		sampler->setWrapMode(1, RT_WRAP_REPEAT);
		sampler->setWrapMode(2, RT_WRAP_REPEAT);
		sampler->setIndexingMode(RT_TEXTURE_INDEX_NORMALIZED_COORDINATES);
		sampler->setReadMode(RT_TEXTURE_READ_NORMALIZED_FLOAT);
		sampler->setMaxAnisotropy(1.0f);
		sampler->setFilteringModes(RT_FILTER_LINEAR, RT_FILTER_LINEAR, RT_FILTER_NONE);
		textures[ret.getID()] = sampler;
	}

	return textures[ret.getID()];
}

void OptixTracer::createSceneGraph(const aiScene *scene)
{
	std::string pathMesh = pathToPTX("triangle_mesh.cu");
	Program intersection = ctx->createProgramFromPTXFile(pathMesh, "mesh_intersect");
	Program meshAABB = ctx->createProgramFromPTXFile(pathMesh, "mesh_bounds");

	std::string pathShaders = pathToPTX("shaders.cu");
	Program closestHit = ctx->createProgramFromPTXFile(pathShaders, "closest_hit_mesh");
	Program anyHit = ctx->createProgramFromPTXFile(pathShaders, "any_hit_solid");

	vector<GeometryInstance> gis;

	for(int k = 0; k < scene->mNumMeshes; ++k)
	{
		const aiMesh *mesh = scene->mMeshes[k];
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
		gMesh->setBoundingBoxProgram(meshAABB);
		gMesh->setIntersectionProgram(intersection);

		gMesh["vertex_buffer"]->setBuffer(getBufferFromVector(vertexData, RT_FORMAT_FLOAT3));
		gMesh["normal_buffer"]->setBuffer(getBufferFromVector(normalData, RT_FORMAT_FLOAT3));
		gMesh["texcoord_buffer"]->setBuffer(getBufferFromVector(uvData, RT_FORMAT_FLOAT2));
		gMesh["index_buffer"]->setBuffer(getBufferFromVector(indices, RT_FORMAT_INT3));

		Material material = ctx->createMaterial();
		material->setClosestHitProgram(0, closestHit);
		material->setAnyHitProgram(1, anyHit);

		const aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material["Ka"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material["Kd"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material["Ks"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
		material["reflectivity"]->setFloat(make_float3(color.r, color.g, color.b));

		float phongexp;
		mat->Get(AI_MATKEY_SHININESS, phongexp);
		material["phong_exp"]->setFloat(phongexp);

		material["ambient_map"]->setTextureSampler(getTexture(mat, aiTextureType_AMBIENT));
		material["diffuse_map"]->setTextureSampler(getTexture(mat, aiTextureType_DIFFUSE));
		material["specular_map"]->setTextureSampler(getTexture(mat, aiTextureType_SPECULAR));

		GeometryInstance inst = ctx->createGeometryInstance();
		inst->setMaterialCount(1);
		inst->setGeometry(gMesh);
		inst->setMaterial(0, material);
		//inst->setMaterial(0, materials[MIRROR]);
		//inst->setMaterial(0, materials[GLASS]);

		gis.push_back(inst);
	}

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

void OptixTracer::trace()
{
	try
	{
		ctx->launch(0, Environment::get().bufferWidth, Environment::get().bufferHeight);
	}
	catch(Exception &ex)
	{
		printf("%s\n", ex.what());
		exit(0);
	}
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
