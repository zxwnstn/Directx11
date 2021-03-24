
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

cbuffer Camera : register(b2)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int padding_;
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
};

Output main(Input input)
{
	Output output;

	//Transform
	float4 pos = float4(input.position, 1.0f);
	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);
	output.position.xyz += CPosition;
	output.position = mul(output.position, WorldMatrix);
	output.tex = input.tex;

	return output;
}