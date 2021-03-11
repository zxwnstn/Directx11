
Texture2D Texture : register(t0);
SamplerState SampleType;

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

cbuffer CBUFFER : register(b0)
{
	int4 aa;
}

float4 main(Input input) : SV_TARGET
{
	float4 pixel = Texture.Sample(SampleType, input.tex);
	return pixel;
	//float4 pixel = Texture.Sample(SampleType, input.tex);
	//float p = pixel.r;
	////return float4(p, p, p, 1.0f);
	//return float4(1.0f, 0.0f, 1.0f, 1.0f);
}