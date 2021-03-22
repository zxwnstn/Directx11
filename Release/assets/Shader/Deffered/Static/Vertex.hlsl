
cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float4 EAmbient;
};

cbuffer Camera : register(b1)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int Cpadding_;
};

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

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;

	float3 gAmbient : AMBIENT;
	float3 worldPosition : WORLD_POS;
};

Output main(Input input)
{	
	Output output;

	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);
	
	pos = output.position;
	pos.x = pos.x / pos.w;
	pos.y = pos.y / pos.w;
	pos.z = pos.z / pos.w;
	output.worldPosition = pos;

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, CView);
	output.position = mul(output.position, CProjection);

	//Pixel Inputs
	output.gAmbient = EAmbient;
	output.MaterialIndex = input.MaterialIndex;
	output.tex = input.tex;
	output.normal =		mul(input.normal,   Rotate);
	output.binormal =	mul(input.binormal, Rotate);
	output.tangent =	mul(input.tangent,	Rotate);
	
	return output;
}