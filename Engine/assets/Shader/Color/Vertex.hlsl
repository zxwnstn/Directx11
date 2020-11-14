
cbuffer Camera : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
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
	float4 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Output main(Input input)
{
	Output output;

	input.position.w = 1.0f;

	output.position = mul(input.position, Translate);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Scale);

	output.position = mul(output.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	//output.position = input.position;
	output.color = input.color;

	return output;
}