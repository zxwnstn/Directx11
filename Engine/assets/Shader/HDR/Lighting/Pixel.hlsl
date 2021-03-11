Texture2D HDRTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ToneMapFactor : register(b0)
{
	float WhiteSqr;
	float MiddleGray;
	float AverageLum;
	float padding;
};

struct Input
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

static const float3 BT601 = float3(0.299, 0.587, 0.114);

float4 ReinHardToneMapping(float3 color)
{
	float scaledLum = dot(color, BT601);
	scaledLum = scaledLum * MiddleGray / AverageLum;

	float compressedLum = scaledLum * (1.0f + scaledLum / WhiteSqr) / (1.0f + scaledLum);

	color = color * compressedLum;

	return float4(color, 1.0f);
}

float3 ReinhardToneMapping(float3 color)
{


}


float4 main(Input input) : SV_TARGET
{
	float3 color = HDRTexture.Sample(SampleType, input.UV).xyz;
	//return float4(1.0f, 0.0f, 1.0f, 1.0f);
	return ReinHardToneMapping(color);
}