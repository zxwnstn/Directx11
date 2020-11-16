
Texture2D shaderTexture[3];


SamplerState SampleType;

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 tintColor : COLOR;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 main(Input input) : SV_TARGET
{
	float4 textureColor;
	textureColor = shaderTexture[2].Sample(SampleType, input.tex);

	return textureColor;
}