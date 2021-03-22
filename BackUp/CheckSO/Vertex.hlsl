

struct Input
{
	float4 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};


struct Output
{
	float4 Position : SV_POSITION;
};

Output main(Input input)
{
	Output output;
	output.Position = float4(input.Position);
	return output;
}