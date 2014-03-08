#include "Programs.h"
#include "Utils.h"

using namespace optix;


namespace trayc
{
	Program Programs::anyHitSolid;
	Program Programs::closestHitSolid;
	Program Programs::closestHitMesh;
	Program Programs::closestHitGlass;
	Program Programs::anyHitGlass;
	Program Programs::meshBoundingBox;
	Program Programs::meshIntersectNormalMap;
	Program Programs::meshIntersectNoNormalMap;
	Program Programs::boxIntersect;
	Program Programs::boxAABB;
	Program Programs::rayGeneration;
	Program Programs::rayGenerationAA;
	Program Programs::exception;
	Program Programs::envmapMiss;
	Program Programs::gradientMiss;
	Program Programs::solidMiss;
	Program Programs::anyHit;


	void Programs::init(Context &ctx)
	{
		std::string materialPath(Utils::pathToPTX("material_shaders.cu"));
		std::string contextPath(Utils::pathToPTX("context_shaders.cu"));
		std::string meshPath(Utils::pathToPTX("triangle_mesh.cu"));
		std::string pathBox(Utils::pathToPTX("box.cu"));

		rayGeneration = ctx->createProgramFromPTXFile(contextPath, "pinhole_camera");
		rayGenerationAA = ctx->createProgramFromPTXFile(contextPath, "pinhole_camera_AA");
		exception = ctx->createProgramFromPTXFile(contextPath, "exception");
		envmapMiss = ctx->createProgramFromPTXFile(contextPath, "envmap_miss");
		gradientMiss = ctx->createProgramFromPTXFile(contextPath, "gradient_miss");
		solidMiss = ctx->createProgramFromPTXFile(contextPath, "miss");

		anyHitSolid = ctx->createProgramFromPTXFile(materialPath, "any_hit_solid");
		anyHit = ctx->createProgramFromPTXFile(materialPath, "any_hit");
		closestHitSolid = ctx->createProgramFromPTXFile(materialPath, "closest_hit_phong");
		closestHitGlass = ctx->createProgramFromPTXFile(materialPath, "closest_hit_glass");
		anyHitGlass = ctx->createProgramFromPTXFile(materialPath, "any_hit_shadow_glass");
		closestHitMesh = ctx->createProgramFromPTXFile(materialPath, "closest_hit_mesh");

		meshBoundingBox = ctx->createProgramFromPTXFile(meshPath, "mesh_bounds");
		meshIntersectNormalMap = ctx->createProgramFromPTXFile(meshPath, "mesh_intersect_normalmap");
		meshIntersectNoNormalMap = ctx->createProgramFromPTXFile(meshPath, "mesh_intersect_no_normalmap");

		boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");
		boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");
	}
}