

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
	int padding;
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
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	bool UseShadowMap : SHADOWMAP;
};

Output main(Input input)
{	
	Output output;

	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.tex = input.tex;
	output.UseShadowMap = UseShadowMap;

	return output;
}