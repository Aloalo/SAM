#pragma once

#include "OptixTextureHandler.h"
#include <map>
#include <assimp/scene.h>
#include <Camera.h>
#include "Labyrinth.h"
#include "Setting.h"
#include "lights.h"

//handles optix traceing
class OptixTracer
{
public:
	OptixTracer(void);
	~OptixTracer(void);

	void initialize(unsigned int GLBO);

	void addMesh(const Labyrinth &lab);
	void addMesh(const std::string &path, const aiMesh *mesh, const aiMaterial *mat);
	void addMesh(utils::Materials mat, const aiMesh *mesh);
	void addLight(const BasicLight &light);
	void addScene(const std::string &path, const aiScene * scene);
	void addScene(utils::Materials mat, const aiScene * scene);

	void compileSceneGraph();
	void clearSceneGraph();
	void trace();

	BasicLight& getLight(int i);
	void updateLight(int idx);

	optix::Buffer getBuffer();
	void setBufferSize(int w, int h);
	void setCamera(const reng::Camera &cam);

private:
	template<class T>
	optix::Buffer getBufferFromVector(const std::vector<T> &vec, RTformat type);

	std::string getTextureName(const aiMaterial *mat, aiTextureType type); // TODO: maknut

	void createMaterials();
	std::map<int, optix::Material> materials; // TODO: maknut
	
	optix::Context ctx;
	std::vector<optix::GeometryInstance> gis;
	std::vector<BasicLight> lights;

	OptixTextureHandler texHandler;

	Setting<int> maxRayDepth;
	Setting<int> castsShadows;
	Setting<int> useSchlick;
	Setting<int> useInternalReflections;
};

