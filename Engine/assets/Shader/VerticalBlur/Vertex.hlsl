
cbuffer TextureInform : register(b0)
{
	float TextureWidth;
	float TextureHeight;
	float2 padding;
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
	float2 adjacency1 : ADJ1;
	float2 adjacency2 : ADJ2;
	float2 adjacency3 : ADJ3;
	float2 adjacency4 : ADJ4;
	float2 adjacency5 : ADJ5;
	float2 adjacency6 : ADJ6;
	float2 adjacency7 : ADJ7;
	float2 adjacency8 : ADJ8;
	float2 adjacency9 : ADJ9;
};

Output main(Input input)
{
	Output output;

	output.position = float4(input.position, 1.0f);
	output.tex = input.tex;

	float height = 1.0f / TextureHeight;
	output.adjacency1 = input.tex + float2(0.0f, height * -4.0f);
	output.adjacency2 = input.tex + float2(0.0f, height * -3.0f);
	output.adjacency3 = input.tex + float2(0.0f, height * -2.0f);
	output.adjacency4 = input.tex + float2(0.0f, height * -1.0f);
	output.adjacency5 = input.tex + float2(0.0f, height *  0.0f);
	output.adjacency6 = input.tex + float2(0.0f, height *  1.0f);
	output.adjacency7 = input.tex + float2(0.0f, height *  2.0f);
	output.adjacency8 = input.tex + float2(0.0f, height *  3.0f);
	output.adjacency9 = input.tex + float2(0.0f, height *  4.0f);

	return output;
}