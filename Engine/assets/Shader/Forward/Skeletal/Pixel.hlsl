
#define MaxPart 12

Texture2D SpotShadowMap : register(t0);
TextureCube PointShadowMap : register(t1);
Texture2DArray DirShadowMap : register(t2);
Texture2D materialTexture[MaxPart * 3] : register(t3);

SamplerState SampleType : register(s0);
SamplerComparisonState SampleTypePCF : register(s1);

cbuffer LightCam : register(b0)
{
	matrix LView;
	matrix LProjection;
}

cbuffer Light : register(b1)
{
	float4 LPosition;
	float4 LDirection;
	float4 LColor;
	float  LIntensity;
	int    LType;	//0 Directional, 1 Point, 2 Spot
	float  LInnerAng;
	float  LOuterAng;
	float  LRangeRcp;
	int3 LPadding;
};


cbuffer Materials : register(b2)
{
	float4 MAmbient[MaxPart];
	float4 MDiffuse[MaxPart];
	float4 MSpecular[MaxPart];
	float4 MEmmisive[MaxPart];
	float4 MFresnel[MaxPart];
	float4 MShiness[MaxPart / 4];

	//1 $ has Diffuse 2 $ has Normal 4 $ has specular
	int4  MMode[MaxPart / 4];
};

cbuffer Cascaded : register(b3)
{
	matrix ToShadowSpace;
	float4 ToCascadeOffsetX;
	float4 ToCascadeOffsetY;
	float4 ToCascadeScale;
}

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;

	float3 lightToPos : LTP;
	float3 globalAmbient : AMBIENT;
	float3 worldPosition : WPS;
	float3 camvector : CPS;

	bool UseShadowMap : SHADOWMAP;
};

float4 GetMaterialDiffuseMap(int index, float2 tex, int mapMode)
{
	float4 diffuseMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 1))
		return diffuseMap;

	if (index == 0) diffuseMap = materialTexture[0].Sample(SampleType, tex);
	if (index == 1) diffuseMap = materialTexture[3].Sample(SampleType, tex);
	if (index == 2) diffuseMap = materialTexture[6].Sample(SampleType, tex);
	if (index == 3) diffuseMap = materialTexture[9].Sample(SampleType, tex);
	if (index == 4) diffuseMap = materialTexture[12].Sample(SampleType, tex);
	if (index == 5) diffuseMap = materialTexture[15].Sample(SampleType, tex);
	if (index == 6) diffuseMap = materialTexture[18].Sample(SampleType, tex);
	if (index == 7) diffuseMap = materialTexture[21].Sample(SampleType, tex);
	return diffuseMap;
}

float4 GetMaterialNormalMap(int index, float2 tex, int mapMode)
{
	float4 NormalMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 2))
		return NormalMap;

	if (index == 0) NormalMap = materialTexture[1].Sample(SampleType, tex);
	if (index == 1) NormalMap = materialTexture[4].Sample(SampleType, tex);
	if (index == 2) NormalMap = materialTexture[7].Sample(SampleType, tex);
	if (index == 3) NormalMap = materialTexture[10].Sample(SampleType, tex);
	if (index == 4) NormalMap = materialTexture[13].Sample(SampleType, tex);
	if (index == 5) NormalMap = materialTexture[16].Sample(SampleType, tex);
	if (index == 6) NormalMap = materialTexture[19].Sample(SampleType, tex);
	if (index == 7) NormalMap = materialTexture[22].Sample(SampleType, tex);
	return NormalMap;
}

float4 GetMaterialSpecularMap(int index, float2 tex, int mapMode)
{
	float4 SpecularMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 4))
		return SpecularMap;

	if (index == 0) SpecularMap = materialTexture[2].Sample(SampleType, tex);
	if (index == 1) SpecularMap = materialTexture[5].Sample(SampleType, tex);
	if (index == 2) SpecularMap = materialTexture[8].Sample(SampleType, tex);
	if (index == 3) SpecularMap = materialTexture[11].Sample(SampleType, tex);
	if (index == 4) SpecularMap = materialTexture[14].Sample(SampleType, tex);
	if (index == 5) SpecularMap = materialTexture[17].Sample(SampleType, tex);
	if (index == 6) SpecularMap = materialTexture[20].Sample(SampleType, tex);
	if (index == 7) SpecularMap = materialTexture[23].Sample(SampleType, tex);
	return SpecularMap;
}

float CalcDistAttenuation(float dist, float lightRangeRcp)
{
	float attenuation;
	attenuation = saturate(1.0f - dist * lightRangeRcp);
	attenuation *= attenuation;

	return attenuation;
}

float CalcConeAttenuation(float3 lightPos, float3 lightDir)
{
	float attenuation;

	float3 LightTo = -lightPos;
	float3 LVnormal = normalize(lightDir);
	float3 LTnormal = normalize(LightTo);
	float cos = dot(LVnormal, LTnormal);
	attenuation = saturate((cos - LOuterAng) / (LInnerAng - LOuterAng));

	attenuation *= attenuation;

	return attenuation;
}

float CalcSpotShadow(float3 position)
{
	float4 worldPosition;
	worldPosition = mul(float4(position, 1.0f), LView);
	worldPosition = mul(worldPosition, LProjection);

	float3 uv = worldPosition.xyz / worldPosition.w;
	uv.x = uv.x * 0.5f + 0.5f;
	uv.y = -uv.y * 0.5f + 0.5f;

	return SpotShadowMap.SampleCmpLevelZero(SampleTypePCF, uv.xy, uv.z);
}

float CalcPointShadow(float3 ToPixel, float depth)
{
	float3 ToPixelAbs = abs(ToPixel);
	float Z = max(ToPixelAbs.x, max(ToPixelAbs.y, ToPixelAbs.z));
	float Depth = (LProjection[2][2] * Z + LProjection[3][2]) / Z;
	float4 aa = PointShadowMap.Sample(SampleType, ToPixel);

	return PointShadowMap.SampleCmpLevelZero(SampleTypePCF, ToPixel, Depth);
}

float CalcDirShadow(float3 position)
{
	// Transform the world position to shadow space
	float4 posShadowSpace = mul(float4(position, 1.0), ToShadowSpace);

	// Transform the shadow space position into each cascade position
	float4 posCascadeSpaceX = (ToCascadeOffsetX + posShadowSpace.xxxx) * ToCascadeScale;
	float4 posCascadeSpaceY = (ToCascadeOffsetY + posShadowSpace.yyyy) * ToCascadeScale;

	// Check which cascade we are in
	float4 inCascadeX = abs(posCascadeSpaceX) <= 1.0;
	float4 inCascadeY = abs(posCascadeSpaceY) <= 1.0;
	float4 inCascade = inCascadeX * inCascadeY;

	// Prepare a mask for the highest quality cascade the position is in
	float4 bestCascadeMask = inCascade;
	bestCascadeMask.yzw = (1.0 - bestCascadeMask.x) * bestCascadeMask.yzw;
	bestCascadeMask.zw = (1.0 - bestCascadeMask.y) * bestCascadeMask.zw;
	bestCascadeMask.w = (1.0 - bestCascadeMask.z) * bestCascadeMask.w;
	float bestCascade = dot(bestCascadeMask, float4(0.0, 1.0, 2.0, 3.0));

	// Pick the position in the selected cascade
	float3 UVD;
	UVD.x = dot(posCascadeSpaceX, bestCascadeMask);
	UVD.y = dot(posCascadeSpaceY, bestCascadeMask);
	UVD.z = posShadowSpace.z;

	// Convert to shadow map UV values
	UVD.xy = 0.5 * UVD.xy + 0.5;
	UVD.y = 1.0 - UVD.y;

	// Compute the hardware PCF value
	float shadow = DirShadowMap.SampleCmpLevelZero(SampleTypePCF, float3(UVD.xy, bestCascade), UVD.z);

	// set the shadow to one (fully lit) for positions with no cascade coverage
	shadow = saturate(shadow + 1.0 - any(bestCascadeMask));

	return shadow;
}


float4 main(Input input) : SV_TARGET
{
	int materialIndex = input.MaterialIndex;
	int mapMode = MMode[materialIndex / 4][materialIndex % 4];

	//step 1. Get material mapping color
	float4 diffuseMapFirst = GetMaterialDiffuseMap(materialIndex, input.tex, mapMode);
	if (LPadding.x == 1)
	{
		return diffuseMapFirst;
	}
	if (diffuseMapFirst.w < 0.9f) discard;

	float3 diffuseMap = diffuseMapFirst.xyz;
	float3 normalMap = GetMaterialNormalMap(materialIndex, input.tex, mapMode).xyz;
	float3 specularMap = GetMaterialSpecularMap(materialIndex, input.tex, mapMode).xyz;

	float3 Diffuse = MDiffuse[materialIndex].xyz * diffuseMap;
	float3 Specular = MSpecular[materialIndex].xyz * specularMap;

	if (mapMode & 2)
	{
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
		input.normal = normalize(input.normal);
	}

	//Step 2. Calc Halfway Vector
	float LightAttenuation = 1.0f; // no light decrease
	float3 LightVector = -LDirection.xyz;

	if (LType == 1)
	{
		LightAttenuation = CalcDistAttenuation(length(input.lightToPos), LRangeRcp);
		LightVector = -input.lightToPos;
	}
	if (LType == 2)
	{
		LightAttenuation = CalcDistAttenuation(length(input.lightToPos), LRangeRcp)
			* CalcConeAttenuation(input.lightToPos, LightVector);
	}
	float3 LightColor = LColor.xyz * LightAttenuation;
	LightVector = normalize(LightVector);

	//Step 2. Calc Halfway Vector
	float3 NormalProjection = max(dot(input.normal, LightVector), 0.0f) * input.normal;
	float3 HalfVector = NormalProjection - LightVector;
	float3 SpecularVector = normalize(2 * HalfVector + LightVector);

	//Step3. Caclc diffuse, specular factor 
	float df = max(dot(LightVector, input.normal), 0.0f);	//diffuse factor 	

	float shiness = MShiness[materialIndex / 4][materialIndex % 4];
	float nn = max(dot(SpecularVector, input.camvector), 0.0f);
	float sf = pow(nn, shiness);

	//step4. calc shadow
	float ShadowAtt = 1.0f;
	if (input.UseShadowMap)
	{
		if (LType == 0) ShadowAtt = saturate(0.3 + CalcDirShadow(input.worldPosition));
		if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(input.lightToPos, input.position.w));
		if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(input.worldPosition));
	}

	//Step5. Calc finale caculated phong blinn
	float3 finalAmbient = input.globalAmbient * MAmbient[materialIndex];
	//float3 finalDiffuse = df * (Diffuse.xyz * LIntensity * LightColor) + finalAmbient * (ShadowAtt * Diffuse.xyz * LIntensity * LightColor);
	float3 finalDiffuse = (float3(df, df, df) + finalAmbient) * (Diffuse * ShadowAtt * LIntensity * LightColor);
	float3 finalSpecular = sf * (Specular.xyz * LIntensity * LightColor);
	float3 color = finalDiffuse + finalSpecular;


	return float4(color, 1.0f);
}