#include "LabMaterials.h"
#include <Programs.h>
#include <Environment.h>

using namespace optix;
using namespace trayc;
using namespace std;

LabMaterials::LabMaterials(void)
{
}


LabMaterials::~LabMaterials(void)
{
}

Material& LabMaterials::getLabyrinthMaterial(int mat)
{
	return labmat[mat];
}

//void OptixTracer::addMesh(const Labyrinth &lab)
	//{
	//	const vector<Box> &walls = lab.getWalls();
	//	int n = walls.size();
	//	for(int i = 0; i < n; ++i)
	//	{
	//		Geometry box = ctx->createGeometry();
	//		box->setPrimitiveCount(1);
	//		box->setBoundingBoxProgram(Programs::boxAABB);
	//		box->setIntersectionProgram(Programs::boxIntersect);
	//		box["boxmin"]->setFloat(walls[i].boxmin);
	//		box["boxmax"]->setFloat(walls[i].boxmax);
	//		gis.push_back(ctx->createGeometryInstance(box, &matHandler.getLabyrinthMaterial(walls[i].matIdx), 
	//			&matHandler.getLabyrinthMaterial(walls[i].matIdx)+1));
	//	}

	//	std::string pathFloor = Utils::pathToPTX("rectangleAA.cu"); //TODO: texture floor
	//	Geometry floor = ctx->createGeometry();
	//	floor->setPrimitiveCount(1);
	//	floor->setBoundingBoxProgram(ctx->createProgramFromPTXFile(pathFloor, "bounds"));
	//	floor->setIntersectionProgram(ctx->createProgramFromPTXFile(pathFloor, "intersect"));

	//	float rw = lab.getRealWidth(), rh = lab.getRealHeight();
	//	floor["plane_normal"]->setFloat(0.0f, 1.0f, 0.0f);
	//	floor["recmin"]->setFloat(-rw / 2.0f, 0.0f, -rh / 2.0f);
	//	floor["recmax"]->setFloat(rw / 2.0f, 0.0f, rh / 2.0f);

	//	gis.push_back(ctx->createGeometryInstance(floor, &matHandler.getLabyrinthMaterial(MaterialHandler::LabMaterials::WALL), &matHandler.getLabyrinthMaterial(MaterialHandler::LabMaterials::WALL)+1));
	//}

void LabMaterials::createLabMaterials()
{
	Material wallMaterial = ctx->createMaterial();
	wallMaterial->setClosestHitProgram(0, Programs::closestHitSolid);
	wallMaterial->setAnyHitProgram(1, Programs::anyHit);
	wallMaterial["Ka"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["Kd"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
	wallMaterial["phong_exp"]->setFloat(88.0f);
	wallMaterial["ambient_map"]->setTextureSampler(OptixTextureHandler::get().get(Utils::defTexture("opacityDefault.png")));
	labmat[WALL] = wallMaterial;

	Material mirrorMaterial = ctx->createMaterial();
	mirrorMaterial->setClosestHitProgram(0, Programs::closestHitSolid);
	mirrorMaterial->setAnyHitProgram(1, Programs::anyHit);
	mirrorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
	mirrorMaterial["Kd"]->setFloat(0.7f, 0.7f, 0.7f);
	mirrorMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
	mirrorMaterial["ambient_map"]->setTextureSampler(OptixTextureHandler::get().get(Utils::defTexture("opacityDefault.png")));
	mirrorMaterial["phong_exp"]->setFloat(88.0f);
	mirrorMaterial["reflectivity"]->setFloat(0.7f, 0.7f, 0.7f);
	labmat[MIRROR] = mirrorMaterial;

	Material glassMaterial = ctx->createMaterial();
	glassMaterial->setClosestHitProgram(0, Programs::closestHitGlass);
	glassMaterial->setAnyHitProgram(1, Programs::anyHitGlass);
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
	labmat[GLASS] = glassMaterial;
}