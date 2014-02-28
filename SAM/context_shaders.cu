#include "phong.h"

//
// Pinhole camera implementation
//
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, U, , );
rtDeclareVariable(float3, V, , );
rtDeclareVariable(float3, W, , );

rtBuffer<float4, 2> output_buffer;

RT_PROGRAM void pinhole_camera()
{
	float2 screen = make_float2(output_buffer.size());

	float2 d = make_float2(launch_index) / screen * 2.f - 1.f;
	float3 ray_origin = eye;
	float3 ray_direction = normalize(d.x * U + d.y * V + W);

	optix::Ray ray(ray_origin, ray_direction, radiance_ray_type, scene_epsilon);

	PerRayData_radiance prd;
	prd.importance = 1.f;
	prd.depth = 0;

	rtTrace(top_object, ray, prd);

	output_buffer[launch_index] = make_float4(prd.result);
}

//
// Enviormement map
//
rtTextureSampler<float4, 2> envmap;
RT_PROGRAM void envmap_miss()
{
	float theta = atan2f(ray.direction.x, ray.direction.z);
	float phi = M_PIf * 0.5f - acosf(ray.direction.y);
	float u = (theta + M_PIf) * (0.5f * M_1_PIf);
	float v = 0.5f * (1.0f + sinf(phi));
	prd_radiance.result = make_float3(tex2D(envmap, u, v));
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
	prd_radiance.result = 2.0f * phi / M_PIf * (miss_max - miss_min) + miss_min;
}

//
// Set pixel to solid color upon failure
//
rtDeclareVariable(float3, bad_color, , );

RT_PROGRAM void exception()
{
	output_buffer[launch_index] = make_float4(bad_color, 1.0f);
}

