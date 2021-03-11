Texture2D InputA : register(t0);
Texture2D InputB : register(t1);
RWTexture2D<float4> Output : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	Output[DTid.xy] = InputA[DTid.xy] + InputB[DTid.xy];
}