cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float3 EAmbient;
	bool EUseShadowMap;
	float4 Bias;
};

cbuffer Camera : register(b1)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int padding_;
};

cbuffer Transform : register(b2)
{
	matrix Translate;
	matrix Rotate;
	matrix Scale;
};

cbuffer LightPos : register(b3)
{
	float4 LPosition;
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

struct Output
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	float3 lightToPos : LTP;
	float3 globalAmbient : AMBIENT;
	float3 worldPosition : WPS;
	float3 camvector : CV;

	bool UseShadowMap : SHADOWMAP;
};


[domain("tri")]
Output main(HS_ConstantOutput HSConstantData, const OutputPatch<HS_ControlPointOutput, 3> I, float3 f3BarycentricCoords : SV_DomainLocation)
{
	//Output output = (Output)0;
	Output output = (Output)0;

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

	float3 f3BiNormal = I[0].binormal * fWW +
		I[1].binormal * fUU +
		I[2].binormal * fVV +
		HSConstantData.f3N110 * fW * fU +
		HSConstantData.f3N011 * fU * fV +
		HSConstantData.f3N101 * fW * fV;

	float3 f3Tangent = I[0].tangent * fWW +
		I[1].tangent * fUU +
		I[2].tangent * fVV +
		HSConstantData.f3N110 * fW * fU +
		HSConstantData.f3N011 * fU * fV +
		HSConstantData.f3N101 * fW * fV;


	// Normalize the interpolated normal    
	f3Normal = normalize(f3Normal);
	f3BiNormal = normalize(f3BiNormal);
	f3Tangent = normalize(f3Tangent);

	// Linearly interpolate the texture coords
	output.tex = I[0].f2TexCoord * fW + I[1].f2TexCoord * fU + I[2].f2TexCoord * fV;

	// Transform model position with view-projection matrix
	float4 pos = float4(f3Position, 1.0f);
	matrix skinT = I[0].skinTransform * fW + I[1].skinTransform * fU + I[2].skinTransform * fV;
	output.position = mul(pos, skinT);
	output.position = mul(output.position, Scale);
	output.position = mul(output.position, Rotate);
	output.position = mul(output.position, Translate);
	
	pos = output.position;
	pos.x = pos.x / pos.w;
	pos.y = pos.y / pos.w;
	pos.z = pos.z / pos.w;

	output.position = mul(output.position, WorldMatrix);
	output.position = mul(output.position, CView);
	output.position = mul(output.position, CProjection);

	output.MaterialIndex = I[0].MaterialIndex;

	output.normal = mul(f3Normal, mul(skinT, Rotate));
	output.binormal = mul(f3BiNormal, mul(skinT, Rotate));
	output.tangent = mul(f3Tangent, mul(skinT, Rotate));
	
	output.lightToPos = pos.xyz - LPosition;
	output.globalAmbient = EAmbient;
	output.worldPosition = pos.xyz;
	output.camvector = normalize(CPosition - pos.xyz);

	output.UseShadowMap = EUseShadowMap;

	return output;
}