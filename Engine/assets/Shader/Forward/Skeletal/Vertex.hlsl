struct Input
{
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;

	float3 f3Normal     : NORMAL;
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
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;

	float3 f3Normal     : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	matrix skinTransform : SKINT;
};

cbuffer Bone : register(b0)
{
	matrix SkinnedTransform[100];
}

Output main(Input input)
{
	Output output;

	matrix skinTransform = 0;
	skinTransform += SkinnedTransform[input.boneIndices.x] * input.boneWeight.x;
	skinTransform += SkinnedTransform[input.boneIndices.y] * input.boneWeight.y;
	skinTransform += SkinnedTransform[input.boneIndices.z] * input.boneWeight.z;
	skinTransform += SkinnedTransform[input.boneIndices.w] * input.boneWeight.w;
	skinTransform += SkinnedTransform[input.boneIndicesr.x] * input.boneWeightr.x;
	skinTransform += SkinnedTransform[input.boneIndicesr.y] * input.boneWeightr.y;
	skinTransform += SkinnedTransform[input.boneIndicesr.z] * input.boneWeightr.z;
	skinTransform += SkinnedTransform[input.boneIndicesr.w] * input.boneWeightr.w;
	
	output.f3Position = input.f3Position;
	output.f2TexCoord = input.f2TexCoord;
	
	output.f3Normal = input.f3Normal;
	output.binormal = input.binormal;
	output.tangent = input.tangent;

	output.MaterialIndex = input.MaterialIndex;
	output.skinTransform = skinTransform;

	return output;
}
