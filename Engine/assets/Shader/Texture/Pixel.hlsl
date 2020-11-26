
Texture2D Texture;
SamplerState SampleType;

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	float4 pixel = Texture.Sample(SampleType, input.tex);
	return pixel;
}