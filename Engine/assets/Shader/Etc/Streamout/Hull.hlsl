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

HS_CONSTANT_OUTPUT ConstantFunc(InputPatch<HS_OUTPUT, 3> intput)
{
	HS_CONSTANT_OUTPUT output;
	output.TessFactor[0] = output.TessFactor[1] = output.TessFactor[2] = 3.0f;
	output.InsideTessFactor = 3.0f;
	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[patchconstantfunc("ConstantFunc")]
[outputcontrolpoints(3)]
[maxtessfactor(64)]
HS_OUTPUT main(InputPatch<HS_OUTPUT, 3> intput, uint cpid : SV_OutputControlPointID)
{
	HS_OUTPUT output;
	output.Position = intput[cpid].Position;
	output.UV = intput[cpid].UV;
	output.Normal = intput[cpid].Normal;
	return output;
}