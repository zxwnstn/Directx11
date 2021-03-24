
cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float4 EAmbient;
};

cbuffer Transform : register(b1)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

struct Input
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;

	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	int MaterialIndex : MATERIALIDX;
};

Output main(Input input)
{
	Output output;

	//Transform
	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);
	output.position = mul(output.position, WorldMatrix);
	
	//Pixel Inputs
	output.MaterialIndex = input.MaterialIndex;
	output.tex = input.tex;

	return output;
}