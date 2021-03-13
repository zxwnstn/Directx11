
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

cbuffer Bone : register(b4)
{
	matrix SkinnedTransform[100];
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
	float3 globalAmbient : AMBIENT;

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

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
	pos.x = pos.x / pos.w;
	pos.y = pos.y / pos.w;
	pos.z = pos.z / pos.w;
	output.lightToPos = pos.xyz - LPosition;

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);


	//Pixel Inputs
	output.globalAmbient = EAmbient;
	output.MaterialIndex = input.MaterialIndex;
	output.tex = input.tex;
	output.normal =		mul(input.normal, mul(skinTransform,   Rotate));
	output.binormal =	mul(input.binormal, mul(skinTransform, Rotate));
	output.tangent =	mul(input.tangent,	mul(skinTransform, Rotate));

	output.UseShadowMap = UseShadowMap;

	return output;
}