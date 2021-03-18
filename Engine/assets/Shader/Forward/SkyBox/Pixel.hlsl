
Texture2D skyTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer SkyBoxInfo : register(b0)
{
	float4 SColor;
}

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	float4 color = skyTexture.Sample(SampleType, input.tex);
	color = color * SColor;
	if (color.x + color.y + color.z == 0.0f)
	{
		return SColor;
	}

	return float4(color.xyz, 1.0f);
}