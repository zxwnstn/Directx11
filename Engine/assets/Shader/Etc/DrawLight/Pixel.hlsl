struct Input
{
	float4 position : SV_POSITION;
};

cbuffer LightColor : register(b0)
{
	float4 LColor;
};

float4 main(Input input) : SV_TARGET
{
	return float4(LColor.x, LColor.y, LColor.z, 1.0f);
}