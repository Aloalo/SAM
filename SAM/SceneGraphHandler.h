#pragma once

#include "SafeOptix.h"
#include <vector>

namespace trayc
{
	class SceneGraphHandler
	{
	public:
		SceneGraphHandler(void);
		~SceneGraphHandler(void);

		void add(optix::GeometryGroup gg, const std::vector<optix::Transform> &trans);
		void add(optix::GeometryGroup gg, optix::Transform trans);

	private:
		optix::Group topObject, topShadower;
		optix::GeometryGroup staticGeometry;

		std::vector<optix::Transform> transforms;
		std::vector<optix::GeometryGroup> staticGG;
		std::vector<optix::GeometryInstance> gis;

		std::vector<std::vector<int> > conectivty;
	};
}