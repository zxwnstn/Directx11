Texture2D HDRTexture : register(t0);
RWStructuredBuffer<float> Result : register(u0);

groupshared float accum[256];

static const float4 BT601 = float4(0.299, 0.587, 0.114, 0.0);

cbuffer DispatchInfo : register(b0)
{
	uint4 info; // x - dispatch x, y - dispatch y
};

[numthreads(16, 16, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	float4 color = 
		HDRTexture.Load(uint3((DTid.xy                                 ),  0.0)) +
		HDRTexture.Load(uint3((DTid.xy + uint2(16 * info.x, 0          )), 0.0)) +
		HDRTexture.Load(uint3((DTid.xy + uint2(0          , 16 * info.y)), 0.0)) +
		HDRTexture.Load(uint3((DTid.xy + uint2(16 * info.x, 16 * info.y)), 0.0));

	accum[GI] = dot(color, BT601);
	GroupMemoryBarrierWithGroupSync();

	if (GI < 128)
	{
		accum[GI] += accum[GI + 128];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 64)
	{
		accum[GI] += accum[GI + 64];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 32)
	{
		accum[GI] += accum[GI + 32];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 16)
	{
		accum[GI] += accum[GI + 16];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 8)
	{
		accum[GI] += accum[GI + 8];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 4)
	{
		accum[GI] += accum[GI + 4];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 2)
	{
		accum[GI] += accum[GI + 2];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI < 1)
	{
		accum[GI] += accum[GI + 1];
	}
	GroupMemoryBarrierWithGroupSync();

	if (GI == 0)
	{
		if (accum[0] == 0.0f)
		{
			Result[Gid.y * info.x + Gid.x] = 1.0f;
		}
		else
		{
			Result[Gid.y * info.x + Gid.x] = accum[0];
		}
	}

}