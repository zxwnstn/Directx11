struct Input
{
	float3 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct Output
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

Output main(Input input)
{
	Output O;
	O.Position = float4(input.Position, 1.0f);
	O.UV = input.UV;

	return O;
}