#include "Programs.h"
#include "Utils.h"

using namespace optix;
using namespace utils;

Program Programs::anyHitSolid;
Program Programs::closestHitSolid;
Program Programs::closestHitMesh;
Program Programs::closestHitGlass;
Program Programs::anyHitGlass;
Program Programs::meshBoundingBox;
Program Programs::meshIntersect;
Program Programs::boxIntersect;
Program Programs::boxAABB;
Program Programs::rayGeneration;
Program Programs::exception;
Program Programs::envmapMiss;
Program Programs::gradientMiss;
Program Programs::solidMiss;
Program Programs::anyHitTransparent;
Program Programs::closestHitTransparent;

void Programs::init(Context &ctx)
{
	std::string materialPath(pathToPTX("material_shaders.cu"));
	std::string contextPath(pathToPTX("context_shaders.cu"));
	std::string meshPath(pathToPTX("triangle_mesh.cu"));
	std::string pathBox(pathToPTX("box.cu"));

	rayGeneration = ctx->createProgramFromPTXFile(contextPath, "pinhole_camera");
	exception = ctx->createProgramFromPTXFile(contextPath, "exception");
	envmapMiss = ctx->createProgramFromPTXFile(contextPath, "envmap_miss");
	gradientMiss = ctx->createProgramFromPTXFile(contextPath, "gradient_miss");
	solidMiss = ctx->createProgramFromPTXFile(contextPath, "miss");

	anyHitSolid = ctx->createProgramFromPTXFile(materialPath, "any_hit_solid");
	closestHitSolid = ctx->createProgramFromPTXFile(materialPath, "closest_hit_phong");
	closestHitGlass = ctx->createProgramFromPTXFile(materialPath, "closest_hit_glass");
	anyHitGlass = ctx->createProgramFromPTXFile(materialPath, "any_hit_shadow_glass");
	closestHitMesh = ctx->createProgramFromPTXFile(materialPath, "closest_hit_mesh");
	anyHitTransparent = ctx->createProgramFromPTXFile(materialPath, "any_hit_transparent");
	closestHitTransparent = ctx->createProgramFromPTXFile(materialPath, "closest_hit_transparent_mesh");

	meshBoundingBox = ctx->createProgramFromPTXFile(meshPath, "mesh_bounds");
	meshIntersect = ctx->createProgramFromPTXFile(meshPath, "mesh_intersect");

	boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");
	boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");
}