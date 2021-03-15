cbuffer TFactor : register(b0)
{
	float4 tessFactor;
};

struct HS_Input
{
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;
	
	float3 f3Normal     : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	matrix skinTransform : SKINT;
};

struct HS_ControlPointOutput
{
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;
	
	float3 f3Normal     : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	matrix skinTransform : SKINT;
};

struct HS_ConstantOutput
{
	// Tess factor for the FF HW block
	float fTessFactor[3]    : SV_TessFactor;
	float fInsideTessFactor : SV_InsideTessFactor;

	// Geometry cubic generated control points
	float3 f3B210    : POSITION3;
	float3 f3B120    : POSITION4;
	float3 f3B021    : POSITION5;
	float3 f3B012    : POSITION6;
	float3 f3B102    : POSITION7;
	float3 f3B201    : POSITION8;
	float3 f3B111    : CENTER;

	// Normal quadratic generated control points
	float3 f3N110    : NORMAL3;
	float3 f3N011    : NORMAL4;
	float3 f3N101    : NORMAL5;
};

HS_ConstantOutput HS_PNTrianglesConstant(InputPatch<HS_Input, 3> I)
{
	HS_ConstantOutput O = (HS_ConstantOutput)0;

	O.fTessFactor[0] = O.fTessFactor[1] = O.fTessFactor[2] = tessFactor;

	// Assign Positions
	float3 f3B003 = I[0].f3Position;
	float3 f3B030 = I[1].f3Position;
	float3 f3B300 = I[2].f3Position;
	
	// And Normals
	float3 f3N002 = I[0].f3Normal;
	float3 f3N020 = I[1].f3Normal;
	float3 f3N200 = I[2].f3Normal;

	// Compute the cubic geometry control points
	// Edge control points
	O.f3B210 = ((2.0f * f3B003) + f3B030 - (dot((f3B030 - f3B003), f3N002) * f3N002)) / 3.0f;
	O.f3B120 = ((2.0f * f3B030) + f3B003 - (dot((f3B003 - f3B030), f3N020) * f3N020)) / 3.0f;
	O.f3B021 = ((2.0f * f3B030) + f3B300 - (dot((f3B300 - f3B030), f3N020) * f3N020)) / 3.0f;
	O.f3B012 = ((2.0f * f3B300) + f3B030 - (dot((f3B030 - f3B300), f3N200) * f3N200)) / 3.0f;
	O.f3B102 = ((2.0f * f3B300) + f3B003 - (dot((f3B003 - f3B300), f3N200) * f3N200)) / 3.0f;
	O.f3B201 = ((2.0f * f3B003) + f3B300 - (dot((f3B300 - f3B003), f3N002) * f3N002)) / 3.0f;
	
	// Center control point
	float3 f3E = (O.f3B210 + O.f3B120 + O.f3B021 + O.f3B012 + O.f3B102 + O.f3B201) / 6.0f;
	float3 f3V = (f3B003 + f3B030 + f3B300) / 3.0f;
	O.f3B111 = f3E + ((f3E - f3V) / 2.0f);

	// Compute the quadratic normal control points, and rotate into world space
	float fV12 = 2.0f * dot(f3B030 - f3B003, f3N002 + f3N020) / dot(f3B030 - f3B003, f3B030 - f3B003);
	O.f3N110 = normalize(f3N002 + f3N020 - fV12 * (f3B030 - f3B003));
	float fV23 = 2.0f * dot(f3B300 - f3B030, f3N020 + f3N200) / dot(f3B300 - f3B030, f3B300 - f3B030);
	O.f3N011 = normalize(f3N020 + f3N200 - fV23 * (f3B300 - f3B030));
	float fV31 = 2.0f * dot(f3B003 - f3B300, f3N200 + f3N002) / dot(f3B003 - f3B300, f3B003 - f3B300);
	O.f3N101 = normalize(f3N200 + f3N002 - fV31 * (f3B003 - f3B300));


	// Inside tess factor is just the average of the edge factors
	O.fInsideTessFactor = (O.fTessFactor[0] + O.fTessFactor[1] + O.fTessFactor[2]) / 3.0f;

	return O;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("HS_PNTrianglesConstant")]
[outputcontrolpoints(3)]
[maxtessfactor(64)]
HS_ControlPointOutput main(InputPatch<HS_Input, 3> I, uint uCPID : SV_OutputControlPointID)
{
	HS_ControlPointOutput O = (HS_ControlPointOutput)0;

	// Just pass through inputs = fast pass through mode triggered
	O.f3Position = I[uCPID].f3Position;
	
	O.f3Normal = I[uCPID].f3Normal;
	O.binormal = I[uCPID].binormal;
	O.tangent = I[uCPID].tangent;

	O.f2TexCoord = I[uCPID].f2TexCoord;
	O.MaterialIndex = I[uCPID].MaterialIndex;
	
	O.skinTransform = I[uCPID].skinTransform;

	return O;
}
