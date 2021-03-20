
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

struct GBuffer
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal  : SV_TARGET1;
	float4 Ambient : SV_TARGET2;
	float4 WorldPosition : SV_TARGET3;
	float4 Misc : SV_TARGET4;
};

GBuffer main(Input input) : SV_TARGET
{
	GBuffer output;

	float4 color = skyTexture.Sample(SampleType, input.tex);
	color = color * SColor;
	if (color.x + color.y + color.z == 0.0f)
	{
		output.Diffuse = SColor;
	}
	output.Diffuse = color;
	output.Ambient.w = 0.0f;

	return output;
}