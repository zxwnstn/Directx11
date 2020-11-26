
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

cbuffer Bone : register(b3)
{
	matrix SkinnedTransform[100];
}

cbuffer Light2 : register(b4)
{
	matrix LightView;
	matrix LightProjection;
}

struct Input
{
	float3 position : POSITION;
	float2 tex : TEXCOORD;

	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	float4 boneWeight  : WEIGHT;
	float4 boneWeightr : WEIGHTR;
	uint4 boneIndices  : BONEINDICES;
	uint4 boneIndicesr : BONEINDICESR;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	int MaterialIndex : MATERIALIDX;

	bool CreateShadowMap : SHADOWMAP;
	float4 lightViewPosition : LPOSITION;
	float4 depthposition : POS;
	float4 bias : BIAS;
};

Output main(Input input)
{	
	Output output;

	//Transform
	matrix skinTransform = 0;
	skinTransform += SkinnedTransform[input.boneIndices.x] * input.boneWeight.x;
	skinTransform += SkinnedTransform[input.boneIndices.y] * input.boneWeight.y;
	skinTransform += SkinnedTransform[input.boneIndices.z] * input.boneWeight.z;
	skinTransform += SkinnedTransform[input.boneIndices.w] * input.boneWeight.w;
	skinTransform += SkinnedTransform[input.boneIndicesr.x] * input.boneWeightr.x;
	skinTransform += SkinnedTransform[input.boneIndicesr.y] * input.boneWeightr.y;
	skinTransform += SkinnedTransform[input.boneIndicesr.z] * input.boneWeightr.z;
	skinTransform += SkinnedTransform[input.boneIndicesr.w] * input.boneWeightr.w;

	float4 pos = float4(input.position, 1.0f);
	output.position = mul(pos, skinTransform);

	output.position = mul(output.position, Scale);
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