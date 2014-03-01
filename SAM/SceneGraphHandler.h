#pragma once

#include "SafeOptix.h"
#include <vector>

class SceneGraphHandler
{
public:
	SceneGraphHandler(void);
	~SceneGraphHandler(void);

private:
	optix::Group topObject;
	optix::Group dynamicGeometry;
	optix::GeometryGroup staticGeometry;

	std::vector<optix::Transform> transforms;
	std::vector<optix::GeometryGroup> staticGG;
	std::vector<optix::GeometryInstance> gis;

	std::vector<std::vector<int> > conectivty;
};
