
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

struct Input
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
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

	output.position = mul(input.position, Projection);
	output.position = mul(output.position, View);
	output.position = mul(output.position, World);

	output.tex = input.tex;

	return output;
}