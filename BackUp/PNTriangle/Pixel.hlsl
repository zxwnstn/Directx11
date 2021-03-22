
struct DS_Output
{
	float4 f4Position   : SV_Position;
	float2 f2TexCoord   : TEXCOORD0;
	float4 f4Diffuse    : COLOR0;
};

float4 main(DS_Output I) : SV_TARGET
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}