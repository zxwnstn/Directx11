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
	float3 f3Normal      : NORMAL;
	float2 f2TexCoord    : TEXCOORD;
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

struct DS_Output
{
	float4 f4Position   : SV_Position;
	float2 f2TexCoord   : TEXCOORD0;
	float4 f4Diffuse    : COLOR0;
};

[domain("tri")]
DS_Output main(HS_ConstantOutput HSConstantData, const OutputPatch<HS_ControlPointOutput, 3> I, float3 f3BarycentricCoords : SV_DomainLocation)
{
	DS_Output O = (DS_Output)0;

	// The barycentric coordinates
	float fU = f3BarycentricCoords.x;
	float fV = f3BarycentricCoords.y;
	float fW = f3BarycentricCoords.z;

	// Precompute squares and squares * 3 
	float fUU = fU * fU;
	float fVV = fV * fV;
	float fWW = fW * fW;
	float fUU3 = fUU * 3.0f;
	float fVV3 = fVV * 3.0f;
	float fWW3 = fWW * 3.0f;

	// Compute position from cubic control points and barycentric coords
	float3 f3Position = I[0].f3Position * fWW * fW +
		I[1].f3Position * fUU * fU +
		I[2].f3Position * fVV * fV +
		HSConstantData.f3B210 * fWW3 * fU +
		HSConstantData.f3B120 * fW * fUU3 +
		HSConstantData.f3B201 * fWW3 * fV +
		HSConstantData.f3B021 * fUU3 * fV +
		HSConstantData.f3B102 * fW * fVV3 +
		HSConstantData.f3B012 * fU * fVV3 +
		HSConstantData.f3B111 * 6.0f * fW * fU * fV;

	// Compute normal from quadratic control points and barycentric coords
	float3 f3Normal = I[0].f3Normal * fWW +
		I[1].f3Normal * fUU +
		I[2].f3Normal * fVV +
		HSConstantData.f3N110 * fW * fU +
		HSConstantData.f3N011 * fU * fV +
		HSConstantData.f3N101 * fW * fV;

	// Normalize the interpolated normal    
	f3Normal = normalize(f3Normal);

	// Linearly interpolate the texture coords
	O.f2TexCoord = I[0].f2TexCoord * fW + I[1].f2TexCoord * fU + I[2].f2TexCoord * fV;

	// Calc diffuse color    
	O.f4Diffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
	/*O.f4Diffuse.rgb = g_f4MaterialDiffuseColor * g_f4LightDiffuse * max(0, dot(f3Normal, g_f4LightDir.xyz)) + g_f4MaterialAmbientColor;
	O.f4Diffuse.a = 1.0f;*/

	// Transform model position with view-projection matrix
	O.f4Position = mul(float4(f3Position.xyz, 1.0), View);
	O.f4Position = mul(O.f4Position, Projection);

	return O;
}