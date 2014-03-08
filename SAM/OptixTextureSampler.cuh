#pragma once

#include "SafeOptix.h"
#include <optixu/optixu_math_namespace.h>
#include "helper.h"

//const int MAXTEXEXP = 12;
//rtTextureSampler<uchar4, 2, cudaReadModeNormalizedFloat> mipmaps[MAXTEXEXP];
//
//__device__ float4 texture(const float2 &UV)
//{
//	return make_float4(0.0f);
//}

struct OptixTextureSampler
{
	

	/*__host__ float4 ctof(const uchar4 &x)
	{
		return make_float4(x.x, x.y, x.z, x.w);
	}

	__host__ uchar4 ftoc(const float4 &x)
	{
		return make_uchar4(x.x, x.y, x.z, x.w);
	}*/

	/*__host__ void set(optix::Context &ctx, optix::Material &mat, uchar4 *image, int size, float anisotropy);
	//{
		/*uchar4 *img = new uchar4[size * size];
		memcpy(img, image, size * size * sizeof(uchar4));
		uchar4 *newImg = new uchar4[size * size / 4];;

		while(size > 1)
		{
			for(int i = 0; i < size; i += 2)
				for(int j = 0; j < size; j += 2)
					newImg[i * size / 2 + j / 2] = ftoc(
					(ctof(img[i * size + j]) + 
					ctof(img[i * size + j+1]) +
					ctof(img[(i+1) * size + j]) +
					ctof(img[(i+1) * size + j+1])) / 4.0f);

			Buffer buff = ctx->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_BYTE4, size / 2, size / 2);
			memcpy(buff->map(), newImg, size * size / 4 * sizeof(uchar4));
			buff->unmap();
			TextureSampler sampler = ctx->createTextureSampler();
			sampler->setBuffer(0, 1, buff);

			Variable var;
			var->setTextureSampler(sampler);
			size /= 2;

			uchar4 *ptr = img;
			img = newImg;
			newImg = ptr;
		}

		delete[] img;
		delete[] newImg;*/
	//}
};