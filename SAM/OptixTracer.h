#pragma once

#include "MaterialHandler.h"
#include "AccelHandler.h"
#include <assimp/scene.h>
#include <Camera.h>
#include <Setting.h>
#include "lights.h"

namespace trayc
{
	//handles optix traceing
	class OptixTracer
	{
	public:
		OptixTracer(void);
		~OptixTracer(void);

		void initialize(unsigned int GLBO);
		
		void addScene(const std::string &path, const aiScene *scene);
		void addMesh(const std::string &path, const aiMesh *mesh, const aiMaterial *mat);
		
		void addScene(const optix::Material &mat, const aiScene *scene);
		void addMesh(const optix::Material &mat, const aiMesh *mesh);

		void addGeometryInstance(const optix::GeometryInstance &gi);
		void addLight(const BasicLight &light);

		void compileSceneGraph();
		void clearSceneGraph();
		void trace(int entryPoint);

		BasicLight& getLight(int i);
		void updateLight(int idx);

		optix::Buffer getBuffer();
		void setBufferSize(int w, int h);
		void setCamera(const reng::Camera &cam);
		void renderToPPM(const std::string &name);

	private:
		template<class T>
		optix::Buffer getBufferFromVector(const std::vector<T> &vec, RTformat type);
		optix::Geometry getGeometry(const aiMesh *mesh, const aiMaterial *mat = NULL, const std::string &path = Utils::defTexture(""));

		AccelHandler accelHandler;

		std::vector<optix::GeometryInstance> gis;
		std::vector<BasicLight> lights;

		reng::Setting<int> maxRayDepth;
		reng::Setting<int> renderingDivisionLevel;
		reng::Setting<int> castsShadows;
		reng::Setting<int> useSchlick;
		reng::Setting<int> useInternalReflections;
		reng::Setting<int> MSAA;
	};
};
