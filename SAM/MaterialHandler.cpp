#include "MaterialHandler.h"
#include "Programs.h"
#include "Utils.h"

using namespace std;
using namespace optix;

namespace trayc
{
	MaterialHandler::MaterialHandler(optix::Context &ctx)
		: ctx(ctx), texHandler(ctx)
	{
	}


	MaterialHandler::~MaterialHandler(void)
	{
	}

	Material MaterialHandler::createMaterial(const string &path, const aiMaterial *mat)
	{
		Material material = ctx->createMaterial();

		material->setClosestHitProgram(0, Programs::closestHitMesh);
		material->setAnyHitProgram(1, Programs::anyHitSolid);

		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material["Ka"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material["Kd"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material["Ks"]->setFloat(make_float3(color.r, color.g, color.b));

		mat->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
		material["reflectivity"]->setFloat(make_float3(color.r, color.g, color.b));

		float phongexp;
		mat->Get(AI_MATKEY_SHININESS, phongexp);
		material["phong_exp"]->setFloat(phongexp);

		material["ambient_map"]->setTextureSampler(texHandler.get(getTextureName(mat, aiTextureType_AMBIENT, path, "ambDefault.png")));
		material["diffuse_map"]->setTextureSampler(texHandler.get(getTextureName(mat, aiTextureType_DIFFUSE, path, "diffDefault.png")));
		material["specular_map"]->setTextureSampler(texHandler.get(getTextureName(mat, aiTextureType_SPECULAR, path, "specDefault.png")));

		return material;
	}


	Material& MaterialHandler::getLabyrinthMaterial(int mat)
	{
		return labmat[mat];
	}

	void MaterialHandler::createLabMaterials()
	{
		Material wallMaterial = ctx->createMaterial();
		wallMaterial->setClosestHitProgram(0, Programs::closestHitSolid);
		wallMaterial->setAnyHitProgram(1, Programs::anyHit);
		wallMaterial["Ka"]->setFloat(0.8f, 0.8f, 0.8f);
		wallMaterial["Kd"]->setFloat(0.8f, 0.8f, 0.8f);
		wallMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
		wallMaterial["phong_exp"]->setFloat(88.0f);
		wallMaterial["ambient_map"]->setTextureSampler(texHandler.get(Utils::defTexture("opacityDefault.png")));
		labmat[WALL] = wallMaterial;

		Material mirrorMaterial = ctx->createMaterial();
		mirrorMaterial->setClosestHitProgram(0, Programs::closestHitSolid);
		mirrorMaterial->setAnyHitProgram(1, Programs::anyHit);
		mirrorMaterial["Ka"]->setFloat(0.3f, 0.3f, 0.3f);
		mirrorMaterial["Kd"]->setFloat(0.7f, 0.7f, 0.7f);
		mirrorMaterial["Ks"]->setFloat(0.8f, 0.8f, 0.8f);
		mirrorMaterial["ambient_map"]->setTextureSampler(texHandler.get(Utils::defTexture("opacityDefault.png")));
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

	string MaterialHandler::getTextureName(const aiMaterial *mat, aiTextureType type, const string &path, const string &def) const
	{
		aiString name;
		if(mat->GetTextureCount(type) == 0)
			return Utils::defTexture(def);

		mat->GetTexture(type, 0, &name, NULL, NULL, NULL, NULL, NULL);
		return path + string(name.C_Str());
	}
}