
struct DS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

float4 main(DS_OUTPUT input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}