#include "helper.h"

rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 

rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );
rtDeclareVariable(PerRayData_shadow, prd_shadow,   rtPayload, );

rtDeclareVariable(float, t_hit, rtIntersectionDistance, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );

rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(unsigned int, shadow_ray_type, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );

rtDeclareVariable(float3, ambient_light_color, , );

//
// Pinhole camera implementation
//
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, U, , );
rtDeclareVariable(float3, V, , );
rtDeclareVariable(float3, W, , );

rtDeclareVariable(float3, bad_color, , );
rtBuffer<uchar4, 2> output_buffer;

RT_PROGRAM void pinhole_camera()
{
	size_t2 screen = output_buffer.size();

	float2 d = make_float2(launch_index) / make_float2(screen) * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x * U + d.y * V + W);

	optix::Ray ray(ray_origin, ray_direction, radiance_ray_type, scene_epsilon);

	PerRayData_radiance prd;
	prd.importance = 1.f;
	prd.depth = 0;

	rtTrace(top_object, ray, prd);

	output_buffer[launch_index] = make_color(prd.result);
}

//
// Returns solid color for miss rays
//
rtDeclareVariable(float3, miss_color, , );
RT_PROGRAM void miss()
{
	prd_radiance.result = miss_color;
}

//
// Returns color from [miss_min, miss_max] lineary interpolated across ray inclination
//
rtDeclareVariable(float3, miss_min, , );
rtDeclareVariable(float3, miss_max, , );
RT_PROGRAM void gradient_miss()
{
	float phi = asinf(ray.direction.y);
	prd_radiance.result = 2.0f * phi / pi * (miss_max - miss_min) + miss_min;
}

//
// Terminates and fully attenuates ray after any hit
//
RT_PROGRAM void any_hit_solid()
{
	prd_shadow.attenuation = make_float3(0.0f);
	rtTerminateRay();
}

static __device__ inline float3 get_ffnormal()
{
	float3 world_geo_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));
	float3 world_shade_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
	return faceforward(world_shade_normal, -ray.direction, world_geo_normal);
}

//
//ADS phong shader with shadows
//
rtDeclareVariable(float3, Ka, , );
rtDeclareVariable(float3, Kd, , );
rtDeclareVariable(float3, Ks, , );
rtDeclareVariable(float,  phong_exp, , );
rtBuffer<BasicLight> lights; 

static __device__ inline float3 phong_and_shadows(const float3 &ffnormal, const float3 &hit_point, const float3 &local_Kd)
{
	float3 color = Ka * ambient_light_color;
	for(int i = 0; i < lights.size(); ++i)
	{
		BasicLight light = lights[i];
		float3 L = normalize(light.pos - hit_point);
		float nDl = dot(ffnormal, L);

		if(nDl > 0.0f)
		{
			PerRayData_shadow shadow_prd;
			shadow_prd.attenuation = make_float3(1.0f);
			float Ldist = length(light.pos - hit_point);
			optix::Ray shadow_ray(hit_point, L, shadow_ray_type, scene_epsilon, Ldist);
			rtTrace(top_object, shadow_ray, shadow_prd);

			if(fmaxf(shadow_prd.attenuation) > 0.0f)
			{
				float3 light_color = light.color * shadow_prd.attenuation;
				color += local_Kd * nDl * light_color;

				float3 H = normalize(L - ray.direction);
				float nDh = dot(ffnormal, H);
				if(nDh > 0)
					color += Ks * light.color * powf(nDh, phong_exp);
			}
		}
	}
	return color;
}


RT_PROGRAM void closest_hit_phong()
{
	float3 ffnormal = get_ffnormal();
	float3 hit_point = ray.origin + t_hit * ray.direction;

	prd_radiance.result = phong_and_shadows(ffnormal, hit_point, Kd);
}

//
//ADS phong shader with shadows and procedural tile texture
//

rtDeclareVariable(float3, tile_v0, , );
rtDeclareVariable(float3, tile_v1, , );
rtDeclareVariable(float3, crack_color, , );
rtDeclareVariable(float, crack_width, , );

RT_PROGRAM void closest_hit_phong_tile()
{
	float3 ffnormal = get_ffnormal();
	float3 hit_point = ray.origin + t_hit * ray.direction;

	float v0 = dot(tile_v0, hit_point);
	float v1 = dot(tile_v1, hit_point);
	v0 = v0 - floor(v0);
	v1 = v1 - floor(v1);

	float3 local_Kd;
	if(v0 > crack_width && v1 > crack_width )
		local_Kd = Kd;
	else
		local_Kd = crack_color;

	prd_radiance.result = phong_and_shadows(ffnormal, hit_point, local_Kd);
}

//
//ADS phong shader with shadows and reflections
//
rtDeclareVariable(float3, reflectivity, , );
rtDeclareVariable(float, importance_cutoff, , );
rtDeclareVariable(int, max_depth, , );

RT_PROGRAM void closest_hit_reflection()
{
	float3 ffnormal = get_ffnormal();
	float3 hit_point = ray.origin + t_hit * ray.direction;
	float3 color = phong_and_shadows(ffnormal, hit_point, Kd);
	
	float3 r = schlick(-dot(ffnormal, ray.direction), reflectivity);
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

	prd_radiance.result = color;
}


//
// Transparent object shadows
//
rtDeclareVariable(float3, shadow_attenuation, , );

RT_PROGRAM void any_hit_shadow_glass()
{
  float3 world_normal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
  float nDi = fabs(dot(world_normal, ray.direction));

  prd_shadow.attenuation *= 1 - fresnel_schlick(nDi, 5, 1 - shadow_attenuation, make_float3(1));

  rtIgnoreIntersection();
}

//
// Glass shader
//
rtDeclareVariable(float3, cutoff_color, , );
rtDeclareVariable(float, fresnel_exponent, , );
rtDeclareVariable(float, fresnel_minimum, , );
rtDeclareVariable(float, fresnel_maximum, , );
rtDeclareVariable(float, refraction_index, , );
rtDeclareVariable(float3, refraction_color, , );
rtDeclareVariable(float3, reflection_color, , );
rtDeclareVariable(float3, extinction_constant, , );

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

	// refraction
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
				cos_theta = dot(t, n);

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
	}

	// reflection
	if(prd_radiance.depth < max_depth)
	{
		float3 r = reflect(i, n);

		float importance = prd_radiance.importance * reflection * optix::luminance(reflection_color * beer_attenuation);
		if(importance > importance_cutoff)
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


//
// Set pixel to solid color upon failure
//
RT_PROGRAM void exception()
{
	output_buffer[launch_index] = make_color(bad_color);
}
