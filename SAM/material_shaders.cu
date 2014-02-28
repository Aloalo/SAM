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

//
// Transparent object shadows, no textures
//
rtDeclareVariable(float3, shadow_attenuation, , );

RT_PROGRAM void any_hit_shadow_glass()
{
	float3 world_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	float nDi = fabs(dot(world_normal, ray.direction));

	prd_shadow.attenuation *= 1 - fresnel_schlick(nDi, 5.0f, 1.0f - shadow_attenuation, make_float3(1.0f));

	rtIgnoreIntersection();
}

//
// Glass shader, no textures
//
rtDeclareVariable(float3, cutoff_color, , );
rtDeclareVariable(float, fresnel_exponent, , );
rtDeclareVariable(float, fresnel_minimum, , );
rtDeclareVariable(float, fresnel_maximum, , );
rtDeclareVariable(float, refraction_index, , );
rtDeclareVariable(float3, refraction_color, , );
rtDeclareVariable(float3, reflection_color, , );
rtDeclareVariable(float3, extinction_constant, , );
rtDeclareVariable(int, use_internal_reflections, , );

RT_PROGRAM void closest_hit_glass()
{
	const float3 h = ray.origin + t_hit * ray.direction;
	const float3 n = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal)); // normal
	const float3 i = ray.direction;// incident direction

	float reflection = 1.0f;
	float3 result = make_float3(0.0f);

	float3 beer_attenuation;
	if(dot(n, ray.direction) > 0)
		beer_attenuation = exp(extinction_constant * t_hit);
	else
		beer_attenuation = make_float3(1);

	bool inside = false;

	if(prd_radiance.depth < max_depth)
	{
		float3 t;
		if(refract(t, i, n, refraction_index))
		{
			// check for external or internal reflection
			float cos_theta = dot(i, n);
			if(cos_theta < 0.0f)
				cos_theta = -cos_theta;
			else
			{
				inside = true;
				cos_theta = dot(t, n);
			}

			reflection = fresnel_schlick(cos_theta, fresnel_exponent, fresnel_minimum, fresnel_maximum);

			float importance = prd_radiance.importance * (1.0f - reflection) * optix::luminance(refraction_color * beer_attenuation);
			if(importance > importance_cutoff)
			{
				optix::Ray ray(h, t, radiance_ray_type, scene_epsilon);
				PerRayData_radiance refr_prd;
				refr_prd.depth = prd_radiance.depth+1;
				refr_prd.importance = importance;

				rtTrace(top_object, ray, refr_prd);
				result += (1.0f - reflection) * refraction_color * refr_prd.result;
			}
			else
				result += (1.0f - reflection) * refraction_color * cutoff_color;
		}

		float3 r = reflect(i, n);

		float importance = prd_radiance.importance * reflection * optix::luminance(reflection_color * beer_attenuation);
		if(importance > importance_cutoff && (!inside || (inside && use_internal_reflections)))
		{
			optix::Ray ray(h, r, radiance_ray_type, scene_epsilon);
			PerRayData_radiance refl_prd;
			refl_prd.depth = prd_radiance.depth + 1;
			refl_prd.importance = importance;

			rtTrace(top_object, ray, refl_prd);
			result += reflection * reflection_color * refl_prd.result;
		}
		else
			result += reflection * reflection_color * cutoff_color;
	}

	result = result * beer_attenuation;
	prd_radiance.result = result;
}


rtTextureSampler<float4, 2> ambient_map;        
rtTextureSampler<float4, 2> diffuse_map;
rtTextureSampler<float4, 2> specular_map;
rtTextureSampler<float4, 2> opacity_map;

rtDeclareVariable(float3, texcoord, attribute texcoord, ); 

template<bool transparent>
static __inline__ __device__ void shade_mesh()
{
	float3 direction = ray.direction;
	float3 world_shading_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	float3 world_geometric_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));
	float3 ffnormal = faceforward(world_shading_normal, -direction, world_geometric_normal);
	float3 uv = texcoord;

	if(transparent)
	{
		float4 opacity = tex2D(opacity_map, uv.x, uv.y);
		if(opacity.x < importance_cutoff)
		{
			float3 hit_point = ray.origin + t_hit * ray.direction;
			optix::Ray newray(hit_point, ray.direction, radiance_ray_type, scene_epsilon);
			PerRayData_radiance prd;
			prd.depth = prd_radiance.depth;
			prd.importance = prd_radiance.importance;

			rtTrace(top_object, newray, prd);
			prd_radiance.result = prd.result;
			return;
		}
	}

	float3 pKa = make_float3(tex2D(ambient_map, uv.x, uv.y));
	float3 pKd = make_float3(tex2D(diffuse_map, uv.x, uv.y));
	float3 pKs = make_float3(tex2D(specular_map, uv.x, uv.y));

	phongShade(pKa, pKd, pKs, ffnormal, phong_exp, reflectivity);
}

//
//solid mesh with textures and reflectivity
//
RT_PROGRAM void closest_hit_mesh()
{
	shade_mesh<false>();
}

//
//mesh with alpha transparency (leafs and such)
//
RT_PROGRAM void closest_hit_transparent_mesh()
{
	shade_mesh<true>();
}


template<bool transparent>
static __inline__ __device__ void any_hit()
{
	if(transparent)
	{
		float4 opacity = tex2D(opacity_map, texcoord.x, texcoord.y);
		if(opacity.x < importance_cutoff)
			rtIgnoreIntersection();
	}
	phongShadowed();
}

//
// terminates if ray hits solid part
//
RT_PROGRAM void any_hit_transparent()
{
	any_hit<true>();
}

//
// Terminates and fully attenuates ray after any hit
//
RT_PROGRAM void any_hit_solid()
{
	any_hit<false>();
}