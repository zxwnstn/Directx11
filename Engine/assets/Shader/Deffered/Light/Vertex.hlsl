cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float3 EAmbient;
	bool EUseShadowMap;
	float4 Bias;
};

cbuffer Camera : register(b1)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int padding_;
};

cbuffer Transform : register(b2)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

struct Input
{
	float3 position   : POSITION;
	float2 texCoord   : TEXCOORD;

	float3 normal     : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;
};

float4 main(Input input) : SV_POSITION
{
	float4 pos = float4(input.position, 1.0f);
	pos = mul(pos, Scale);
	pos = mul(pos, Rotate);
	pos = mul(pos, Translate);
	pos = mul(pos, WorldMatrix);
	pos = mul(pos, CView);
	pos = mul(pos, CProjection);

	return pos;
}