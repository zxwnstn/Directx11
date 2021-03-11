StructuredBuffer<unsigned int> Input : register(t0);
RWStructuredBuffer<unsigned int> Output : register(u0);

[numthreads(4, 4, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	Output[DTid.y * 8 + DTid.x] = DTid.y * 8 + DTid.x;
}