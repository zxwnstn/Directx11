#include "pch.h"

#include "MeshInfo.h"

namespace Engine {

	bool Vertex::check()
	{
	#ifdef DEBUG_FEATURE
		for (int i = 0; i < 3; ++i)
		{
			if (!(-1000.0f <= Position.m[i] && Position.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= Normal.m[i] && Normal.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BiNormal.m[i] && BiNormal.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= Tangent.m[i] && Tangent.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
		}

		for (int i = 0; i < 4; ++i)
		{
			if (!(-1000.0f <= BoneWeight.m[i] && BoneWeight.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneIndex.m[i] && BoneIndex.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneWeightr.m[i] && BoneWeightr.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
			if (!(-1000.0f <= BoneIndexr.m[i] && BoneIndexr.m[i] <= 1000.0f))
			{
				__debugbreak();
			}
		}
		return true;
	#endif
		return true;
	}

	void ControlPoint::push(float weight, uint32_t index)
	{
		if (i >= 4)
		{
			BoneWeightr.m[i % 4] = weight;
			BoneIndexr.m[i % 4] = index;
			++i;
			return;
		}
		BoneWeight.m[i] = weight;
		BoneIndex.m[i] = index;
		++i;
	}

}

