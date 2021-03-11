struct HS_Input
{
	float3 f3Position   : POSITION;
};

cbuffer TFactor : register(b0)
{
	float4 tessFactor;
};

struct HS_ControlPointOutput
{
	float3 f3Position    : POSITION;
};

struct HS_ConstantOutput
{
	float fTessFactor[3]    : SV_TessFactor;
	float fInsideTessFactor : SV_InsideTessFactor;
};

HS_ConstantOutput HS_PNTrianglesConstant(InputPatch<HS_Input, 3> I)
{
	HS_ConstantOutput O = (HS_ConstantOutput)0;

	O.fTessFactor[0] = O.fTessFactor[1] = O.fTessFactor[2] = tessFactor.x;
	O.fInsideTessFactor = tessFactor.x;

	return O;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[patchconstantfunc("HS_PNTrianglesConstant")]
[outputcontrolpoints(3)]
HS_ControlPointOutput main(InputPatch<HS_Input, 3> I, uint uCPID : SV_OutputControlPointID)
{
	HS_ControlPointOutput O = (HS_ControlPointOutput)0;

	O.f3Position = I[uCPID].f3Position;

	return O;
}
