
cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float3 EAmbient;
	bool CreateShadowMap;
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

cbuffer Light2 : register(b3)
{
	matrix LightView;
	matrix LightProjection;
}

struct Input
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	
	bool CreateShadowMap : SHADOWMAP;
	float4 lightViewPosition : LPOSITION;
	float4 depthposition : POS;
	float4 bias : BIAS;
};

Output main(Input input)
{	
	Output output;

	float4 pos = float4(input.position, 1.0f);

	output.position = mul(pos, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);
	pos = output.position;

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	//Pixel Inputs
	output.tex = input.tex;
	output.lightViewPosition = mul(pos, WorldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, LightView);
	output.lightViewPosition = mul(output.lightViewPosition, LightProjection);
	output.CreateShadowMap = CreateShadowMap;
	output.depthposition = output.position;
	output.bias = Bias;
	
	return output;
}