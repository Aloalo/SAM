#include "helper.h"

rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 

rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );

rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );


//
// Pinhole camera implementation
//
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, U, , );
rtDeclareVariable(float3, V, , );
rtDeclareVariable(float3, W, , );
rtDeclareVariable(float, magicAngle, , );
rtDeclareVariable(float, aspectRatio, , );

rtDeclareVariable(float3, bad_color, , );
rtBuffer<uchar4, 2> output_buffer;

RT_PROGRAM void pinhole_camera()
{
	size_t2 screen = output_buffer.size();

	float2 d = make_float2(launch_index) / screen.x * 2.f - 1.f;
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
// Returns shading normal as the surface shading result
// 
RT_PROGRAM void closest_hit_radiance()
{
	prd_radiance.result = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal)) * 0.5f + 0.5f;
}


//
// Set pixel to solid color upon failure
//
RT_PROGRAM void exception()
{
	output_buffer[launch_index] = make_color(bad_color);
}
