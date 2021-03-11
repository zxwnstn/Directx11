struct HS_CONSTANT_OUTPUT
{
	float TessFactor[3] : SV_TessFactor;
	float InsideTessFactor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

struct DS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_OUTPUT constantData, const OutputPatch<HS_OUTPUT, 3> intput, float3 barycentricCoords : SV_DomainLocation)
{
	DS_OUTPUT output;

	float3 position = 
		intput[0].Position * barycentricCoords.x +
		intput[1].Position * barycentricCoords.y +
		intput[2].Position * barycentricCoords.z;

	output.Position = float4(position, 1.0f);
	return output;

}