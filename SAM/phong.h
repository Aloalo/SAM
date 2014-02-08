#include <optix_world.h>
#include "helper.h"
#include "lights.h"

rtDeclareVariable(rtObject, top_object, , );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );
rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );
rtDeclareVariable(PerRayData_shadow,   prd_shadow, rtPayload, );

rtDeclareVariable(int, max_depth, , );
rtDeclareVariable(int, use_schlick, , );
rtDeclareVariable(int, cast_shadows, , );

rtBuffer<BasicLight> lights;
rtDeclareVariable(float3, ambient_light_color, , );
rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(unsigned int, shadow_ray_type, , );
rtDeclareVariable(float, importance_cutoff, , );
rtDeclareVariable(float, scene_epsilon, , );

static __device__ void phongShadowed()
{
	prd_shadow.attenuation = optix::make_float3(0);
	rtTerminateRay();
}

static __device__ void phongShade(float3 p_Ka,
							   float3 p_Kd,
							   float3 p_Ks,
							   float3 ffnormal,
							   float  phong_exp,
							   float3 reflectivity)
{
	float3 hit_point = ray.origin + t_hit * ray.direction;
	float3 color = p_Ka * ambient_light_color;

	for(int i = 0; i < lights.size(); ++i)
	{
		BasicLight light = lights[i];
		float attenuation;
		float Ldist;
		float3 L;

		if(light.is_directional) // directional light?
		{
			attenuation = 1.0f;
			L = normalize(light.pos);
			Ldist = FLT_MAX;
		}
		else // point light or spotlight
		{
		    L = normalize(light.pos - hit_point);
			Ldist = length(light.pos - hit_point);
			attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * Ldist + light.attenuation.z * Ldist * Ldist);

			//MASIVNO SE RUSI!!!!
			//if(light.spot_cutoff <= 90.0f) // spotlight?
			//{
			//	float clampedCosine = max(0.0f, dot(-L, light.spot_direction));
			//	if(clampedCosine < cosf(light.spot_cutoff * pi / 180.0f)) // outside of spotlight cone?
			//		attenuation = 0.0f;
			//	else
			//		attenuation = attenuation * powf(clampedCosine, light.spot_exponent);   
			//}
		}

		float nDl = dot(ffnormal, L);

		if(nDl > 0.0f)
		{
			PerRayData_shadow shadow_prd;
			shadow_prd.attenuation = make_float3(1.0f);

			if(cast_shadows && light.casts_shadows)
			{
				optix::Ray shadow_ray(hit_point, L, shadow_ray_type, scene_epsilon, Ldist);
				rtTrace(top_object, shadow_ray, shadow_prd);
			}

			if(fmaxf(shadow_prd.attenuation) > 0.0f)
			{
				float3 light_color = light.color * attenuation * shadow_prd.attenuation;
				color += p_Kd * nDl * light_color;

				float3 H = normalize(L - ray.direction);
				float nDh = dot(ffnormal, H);
				if(nDh > 0.0f)
					color += p_Ks * light_color * powf(nDh, phong_exp);
			}
		}
	}

	if(fmaxf(reflectivity) > 0.0f)
	{
		float3 r;
		if(use_schlick)
			r = schlick(-dot(ffnormal, ray.direction), reflectivity);
		else
			r = reflectivity;

		float importance = prd_radiance.importance * optix::luminance(r);

		//reflection ray
		if(importance > importance_cutoff && prd_radiance.depth < max_depth)
		{
			PerRayData_radiance refl_prd;
			refl_prd.importance = importance;
			refl_prd.depth = prd_radiance.depth+1;
			float3 R = reflect(ray.direction, ffnormal);
			optix::Ray refl_ray(hit_point, R, radiance_ray_type, scene_epsilon);
			rtTrace(top_object, refl_ray, refl_prd);
			color += r * refl_prd.result;
		}
	}

	// pass the color back up the tree
	prd_radiance.result = color;
}