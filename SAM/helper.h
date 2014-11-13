#pragma once

#include <optix.h>
#include <optix_math.h>

using namespace optix;

__constant__ float pi = 3.1415926f;
__constant__ float EPS = 1e-4f;

static __device__ __inline__ bool equals(float x, float y)
{
	return abs(x - y) < EPS;
}

static __device__ __inline__ bool isZero(const float3 &x)
{
	return abs(x.x) < EPS && abs(x.y) < EPS, abs(x.z) < EPS;
}

static __device__ __inline__ bool isZero(float x)
{
	return abs(x) < EPS;
}

static __device__ __inline__ bool isBetween(const float3 &a, const float3 &b, const float3 &x)
{
	if((equals(a.x, x.x) || x.x > a.x) && (equals(b.x, x.x) || x.x < b.x)
		&& (equals(a.y, x.y) || x.y > a.y) && (equals(b.y, x.y) || x.y < b.y)
		 && (equals(a.z, x.z) || x.z > a.z) && (equals(b.z, x.z) || x.z < b.z))
		 return true;
	return false;
}

static __device__ __inline__ float3 exp(const float3 &x)
{
	return make_float3(expf(x.x), expf(x.y), expf(x.z));
}

static __device__ __inline__ float3 schlick(float nDi, const float3 &rgb)
{
	const float r = fresnel_schlick(nDi, 5, rgb.x, 1);
	const float g = fresnel_schlick(nDi, 5, rgb.y, 1);
	const float b = fresnel_schlick(nDi, 5, rgb.z, 1);
	return make_float3(r, g, b);
}

static __device__ __inline__ uchar4 make_color(const float3 &c)
{
	return make_uchar4(static_cast<unsigned char>(__saturatef(c.z)*255.99f),  /* B */
		static_cast<unsigned char>(__saturatef(c.y)*255.99f),  /* G */
		static_cast<unsigned char>(__saturatef(c.x)*255.99f),  /* R */
		255u);                                                 /* A */
}

struct PerRayData_radiance
{
	float3 result;
	float importance;
	int depth;
};

struct PerRayData_shadow
{
	float attenuation;
};

