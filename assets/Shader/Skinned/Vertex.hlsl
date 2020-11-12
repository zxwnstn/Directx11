
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
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 boneWeight : WEIGHT;
	uint4 boneIndices : BONEINDICES;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Output main(Input input)
{	
	Output output;
	//output.position = mul(input.position, translate);
	//output.position = mul(output.position, rotate);
	//output.position = mul(output.position, scale);
	

	/*for (int i = 0; i < 4; ++i)
	{
		output.position += weight[i] * mul(input.position, FinalTransform[input.boneIndices[i]]);
	}*/
	/*float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.boneWeight.x;
	weights[1] = input.boneWeight.y;
	weights[2] = input.boneWeight.z;
	weights[3] = input.boneWeight.w;

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(input.position, FinalTransform[indices[i]]).xyz;
	}
	output.position = float4(posL, 1.0f);*/

	matrix skinTransform = 0;
	skinTransform += FinalTransform[input.boneIndices.x] * input.boneWeight.x;
	skinTransform += FinalTransform[input.boneIndices.y] * input.boneWeight.y;
	skinTransform += FinalTransform[input.boneIndices.z] * input.boneWeight.z;
	skinTransform += FinalTransform[input.boneIndices.w] * input.boneWeight.w;

	output.position = mul(input.position, skinTransform);

	output.position = mul(output.position, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);

	output.position = mul(output.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	output.tex = input.tex;

	return output;
}