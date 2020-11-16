
cbuffer Camera : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 Position;
};

cbuffer Transform : register(b1)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

cbuffer Bone : register(b2)
{
	matrix FinalTransform[100];
}

struct Input
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 tintColor : COlOR;

	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	float4 boneWeight : WEIGHT;
	uint4 boneIndices : BONEINDICES;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 tintColor : COLOR;

	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
};

Output main(Input input)
{	
	Output output;

	//Transform
	matrix skinTransform = 0;
	skinTransform += FinalTransform[input.boneIndices.x] * input.boneWeight.x;
	skinTransform += FinalTransform[input.boneIndices.y] * input.boneWeight.y;
	skinTransform += FinalTransform[input.boneIndices.z] * input.boneWeight.z;
	skinTransform += FinalTransform[input.boneIndices.w] * input.boneWeight.w;

	output.position = mul(float4(input.position, 1.0f), skinTransform);

	output.position = mul(output.position, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	output.position = mul(output.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	//Pixel Inputs
	output.tex = input.tex;

	return output;
}