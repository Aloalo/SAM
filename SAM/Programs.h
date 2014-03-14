#pragma once

#include "SafeOptix.h"

namespace trayc
{
	class Programs
	{
	public:

		static void init(optix::Context &ctx);

		static optix::Program anyHitSolid;
		static optix::Program closestHitMesh;
		static optix::Program closestHitGlass;
		static optix::Program anyHitGlass;

		static optix::Program meshBoundingBox;
		static optix::Program meshIntersectNormalMap;
		static optix::Program meshIntersectNoNormalMap;

		static optix::Program rayGeneration;
		static optix::Program rayGenerationAA;
		static optix::Program exception;
		static optix::Program envmapMiss;
		static optix::Program gradientMiss;
		static optix::Program solidMiss;
	};
}
