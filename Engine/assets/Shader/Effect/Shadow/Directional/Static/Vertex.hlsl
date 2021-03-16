
cbuffer Transform : register(b2)
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


float4 main(Input input) : SV_Position
{
	float4 position = float4(input.position, 1.0f);

	position = mul(position, Scale);
	position = mul(position, Rotate);
	position = mul(position, Translate);

	return position;
}