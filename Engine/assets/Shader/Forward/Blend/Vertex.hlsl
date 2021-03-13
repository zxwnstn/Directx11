
struct Input
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

Output main(Input input)
{
	Output output;
	output.position = float4(input.position, 1.0f);
	output.tex = input.tex;

	return output;
}