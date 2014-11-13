#include "phong.h"

rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 

//
// Transparent object shadows, no textures
//
rtDeclareVariable(float3, shadow_attenuation, , );

RT_PROGRAM void any_hit_glass()
{
	const float3 world_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	const float nDi = fabs(dot(world_normal, ray.direction));

	prd_shadow.attenuation *= 1.0f - optix::fresnel_schlick(nDi, 5.0f, 1.0f - shadow_attenuation, make_float3(1.0f)).x;

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
	const float3 &i = ray.direction;// incident direction

	float reflection = 1.0f;
	float3 result = make_float3(0.0f);

	const float3 beer_attenuation = dot(n, ray.direction) > 0.0f ? 
                                        exp(extinction_constant * t_hit) :
                                        make_float3(1.0f);

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

			const float importance = prd_radiance.importance * (1.0f - reflection) * optix::luminance(refraction_color * beer_attenuation);
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

		const float importance = prd_radiance.importance * reflection * optix::luminance(reflection_color * beer_attenuation);
		if(importance > importance_cutoff && (!inside || (inside && use_internal_reflections)))
		{
			const optix::Ray ray(h, r, radiance_ray_type, scene_epsilon);
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

rtDeclareVariable(float3, Ka, , );
rtDeclareVariable(float3, Kd, , );
rtDeclareVariable(float3, Ks, , );
rtDeclareVariable(float3, reflectivity, , );     
rtTextureSampler<uchar4, 2, cudaReadModeNormalizedFloat> diffuse_map;
rtTextureSampler<uchar4, 2, cudaReadModeNormalizedFloat> specular_map;

rtDeclareVariable(float3, texcoord, attribute texcoord, ); 

//
//solid mesh with textures and reflectivity
//
RT_PROGRAM void closest_hit_mesh()
{
	const float4 pKd = tex2D(diffuse_map, texcoord.x, texcoord.y);
	if(prd_radiance.depth < max_depth && pKd.w < importance_cutoff)
	{
		const optix::Ray newray(ray.origin + t_hit * ray.direction, ray.direction, radiance_ray_type, scene_epsilon);
		prd_radiance.depth++;
		rtTrace(top_object, newray, prd_radiance);

		return;
	}
	
	const float3 world_shading_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	const float3 world_geometric_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));
	const float3 ffnormal = faceforward(world_shading_normal, -ray.direction, world_geometric_normal);

	const float4 pKs = tex2D(specular_map, texcoord.x, texcoord.y);
	
	//phongShade(ffnormal, make_float3(0.0f), make_float3(0.0f), make_float3(0.0f), phong_exp, reflectivity);
	//phongShade(make_float3(abs(ffnormal.x), abs(ffnormal.y), abs(ffnormal.z)), make_float3(0.0f), make_float3(0.0f), make_float3(0.0f), phong_exp, reflectivity);
	phongShade(make_float3(pKd) * Ka, make_float3(pKd) * Kd, make_float3(pKs) * Ks, ffnormal, pKs.w * 255.0f, reflectivity);
}

//
// Terminates and fully attenuates ray after any hit
//
RT_PROGRAM void any_hit_solid()
{
	const float opacity = tex2D(diffuse_map, texcoord.x, texcoord.y).w;
	if(opacity < importance_cutoff)
		rtIgnoreIntersection();
	phongShadowed();
}
