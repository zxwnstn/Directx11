

cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float4 EAmbient;
};

cbuffer LightCam : register(b1)
{
	matrix View;
	matrix Projection;
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

	float4 position = float4(input.position, 1.0f);

	output.position = mul(position, skinTransform);
	output.position = mul(output.position, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	return output;
}