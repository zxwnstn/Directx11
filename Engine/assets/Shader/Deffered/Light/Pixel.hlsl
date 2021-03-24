
cbuffer LightColor : register(b0)
{
	float4 LColor;
};

struct GBuffer
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal  : SV_TARGET1;
	float4 Ambient : SV_TARGET2;
	float4 WorldPosition : SV_TARGET3;
	float4 Misc : SV_TARGET4;
};

GBuffer main() : SV_TARGET
{
	GBuffer output;

	output.Diffuse = LColor;
	output.Ambient.w = 0.0f;
	output.Ambient.x = 0.9f;

	return output;
}