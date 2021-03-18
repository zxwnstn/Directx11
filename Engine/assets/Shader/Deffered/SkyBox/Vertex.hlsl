
cbuffer Transform : register(b0)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

cbuffer Camera : register(b1)
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

	float4 pos = float4(input.position, 1.0f);
	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = output.position + float4(CPosition, 0.0f);
	output.position = mul(output.position, CView);
	output.position = mul(output.position, CProjection);

	output.tex = input.tex;

	return output;
}