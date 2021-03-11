
struct Input
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;

	float3 Normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;
};

struct Output
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

Output main(Input input)
{
	Output output;

	output.Position = input.Position;
	output.UV = input.UV;
	output.Normal = input.Normal;

	return output;
}