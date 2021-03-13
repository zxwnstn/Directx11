
cbuffer Camera : register(b0)
{
	matrix View;
	matrix Projection;
	float3 Position;
	int padding_;
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
};

Output main(Input input)
{
	Output output;

	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	return output;
}