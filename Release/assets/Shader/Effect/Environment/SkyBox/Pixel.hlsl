
#define MaxPart 12

Texture2D SkyBoxTexture : register(t0);
SamplerState SampleType : register(s0);

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	uint index : SV_RenderTargetArrayIndex;
};

float4 main(Input input) : SV_TARGET
{
	return SkyBoxTexture.Sample(SampleType, input.tex);
}