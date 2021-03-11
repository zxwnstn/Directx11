
struct DS_Output
{
	float4 f4Position   : SV_Position;
};

float4 main(DS_Output I) : SV_TARGET
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}