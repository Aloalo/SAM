#pragma once

#include "SafeOptix.h"
#include <string>

namespace trayc
{
	class AccelHandler
	{
	public:
		AccelHandler(void);
		~AccelHandler(void);

		void setMesh(const std::string &name);
		void setAccelCaching(bool onoff);
		void setTraverser(const char *traverser);
		void setBuilder(const char *builder);
		void setRefine(const char *ref);
		void setLargeMesh(bool f);
		void loadAccelCache(optix::GeometryGroup geometry_group);
		void saveAccelCache(optix::GeometryGroup geometry_group);

		bool accel_cache_loaded;
	private:
		std::string getCacheFileName() const;

		std::string filename;

		std::string accel_builder;
		std::string accel_traverser;
		std::string accel_refine;
		bool accel_caching_on;
		bool accel_large_mesh;
	};
}
