#include "phong.h"

rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 

//
//ADS phong shader with shadows and reflections, no textures
//
rtDeclareVariable(float3, Ka, , );
rtDeclareVariable(float3, Kd, , );
rtDeclareVariable(float3, Ks, , );
rtDeclareVariable(float, phong_exp, , );
rtDeclareVariable(float3, reflectivity, , );

RT_PROGRAM void closest_hit_phong()
{
	float3 world_geo_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));
	float3 world_shade_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	float3 ffnormal = faceforward(world_shade_normal, -ray.direction, world_geo_normal);

	phongShade(Ka, Kd, Ks, ffnormal, phong_exp, reflectivity);
}

RT_PROGRAM void any_hit()
{
	phongShadowed();
}
