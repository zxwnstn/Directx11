
cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float3 EAmbient;
	bool UseShadowMap;
	float4 Bias;
};

cbuffer Camera : register(b1)
{
	matrix View;
	matrix Projection;
	float3 Position;
	int padding_;
};

cbuffer Transform : register(b2)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

cbuffer LightPos : register(b3)
{
	float4 LPosition;
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
	float3 globalAmbient : AMBIENT;

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;

	float3 lightToPos : LTP;

	bool UseShadowMap : SHADOWMAP;
};

Output main(Input input)
{	
	Output output;

	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	pos = output.position;
	output.lightToPos = pos.xyz - LPosition;

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	//Pixel Inputs
	output.globalAmbient = EAmbient;
	output.MaterialIndex = input.MaterialIndex;
	output.tex = input.tex;
	output.normal =		mul(input.normal,   Rotate);
	output.binormal =	mul(input.binormal, Rotate);
	output.tangent =	mul(input.tangent,	Rotate);
	
	output.UseShadowMap = UseShadowMap;

	return output;
}