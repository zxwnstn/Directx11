
cbuffer LightColor : register(b0)
{
	float4 LColor;
};

float4 main() : SV_TARGET
{
	return LColor;
}