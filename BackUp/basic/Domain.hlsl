cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float3 EAmbient;
	bool CreateShadowMap;
	float4 Bias;
};

cbuffer Camera : register(b1)
{
	matrix View;
	matrix Projection;
	float3 Position;
	int padding_;
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

struct DS_Output
{
	float4 f4Position   : SV_Position;
};

[domain("tri")]
DS_Output main(HS_ConstantOutput HSConstantData, const OutputPatch<HS_ControlPointOutput, 3> I, float3 f3BarycentricCoords : SV_DomainLocation)
{
	DS_Output O = (DS_Output)0;

	// The barycentric coordinates
	float fU = f3BarycentricCoords.x;
	float fV = f3BarycentricCoords.y;
	float fW = f3BarycentricCoords.z;

	float3 f3Position = I[0].f3Position * fW +
						I[1].f3Position * fU +
						I[2].f3Position * fV;

	//O.f4Position = float4(f3Position, 1.0f);
	//return O;

	// Transform model position with view-projection matrix
	matrix ViewProjection = mul(View, Projection);
	O.f4Position = mul(float4(f3Position, 1.0), ViewProjection);
	//O.f4Position = mul(O.f4Position, Projection);

	return O;
}