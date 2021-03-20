Texture2D Texture : register(t0);
SamplerState SampleType;

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	float4 color = Texture.Sample(SampleType, input.tex);
	return color;
}
