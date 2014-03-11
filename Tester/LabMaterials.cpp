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