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

void Programs::init(Context &ctx)
{
	std::string mainPath(pathToPTX("shaders.cu"));
	std::string meshPath(pathToPTX("triangle_mesh.cu"));
	std::string pathBox(pathToPTX("box.cu"));

	rayGeneration = ctx->createProgramFromPTXFile(mainPath, "pinhole_camera");
	exception = ctx->createProgramFromPTXFile(mainPath, "exception");
	envmapMiss = ctx->createProgramFromPTXFile(mainPath, "envmap_miss");
	gradientMiss = ctx->createProgramFromPTXFile(mainPath, "gradient_miss");
	solidMiss = ctx->createProgramFromPTXFile(mainPath, "miss");

	anyHitSolid = ctx->createProgramFromPTXFile(mainPath, "any_hit_solid");
	closestHitSolid = ctx->createProgramFromPTXFile(mainPath, "closest_hit_phong");
	closestHitGlass = ctx->createProgramFromPTXFile(mainPath, "closest_hit_glass");
	anyHitGlass = ctx->createProgramFromPTXFile(mainPath, "any_hit_shadow_glass");
	meshBoundingBox = ctx->createProgramFromPTXFile(meshPath, "mesh_bounds");
	meshIntersect = ctx->createProgramFromPTXFile(meshPath, "mesh_intersect");
	closestHitMesh = ctx->createProgramFromPTXFile(mainPath, "closest_hit_mesh");

	boxIntersect = ctx->createProgramFromPTXFile(pathBox, "box_intersect");
	boxAABB = ctx->createProgramFromPTXFile(pathBox, "box_bounds");
}