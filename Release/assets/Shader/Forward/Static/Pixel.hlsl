
#define MaxPart 12
#define PI 3.141592f

Texture2D SpotShadowMap : register(t0);
TextureCube PointShadowMap : register(t1);
Texture2DArray DirShadowMap : register(t2);
TextureCube EnvironmentMap : register(t3);
Texture2D materialTexture[MaxPart * 3] : register(t4);

SamplerState SampleType : register(s0);
SamplerComparisonState SampleTypePCF : register(s1);

cbuffer ShadingData : register(b0)
{
	int4 SData1; //x = Lighting, y = shadow, z = diffuse(lambert, half), w = specular(phong blinn) 
	int4 SData2; //x = lambert Pow factor, y = deffered blend factor, z = gamma correction
}

cbuffer LightCam : register(b1)
{
	matrix LView;
	matrix LProjection;
}

cbuffer Light : register(b2)
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


cbuffer Materials : register(b3)
{
	float4 MAmbient[MaxPart];
	float4 MDiffuse[MaxPart];
	float4 MSpecular[MaxPart];
	float4 MEmmisive[MaxPart];
	float4 MFresnel[MaxPart];
	float4 MShiness[MaxPart / 4];
	float4 MRoughness[MaxPart / 4];
	float4 MMetalic[MaxPart / 4];

	//1 $ has Diffuse 2 $ has Normal 4 $ has specular
	int4  MMode[MaxPart / 4];
};

cbuffer Cascaded : register(b4)
{
	matrix ToShadowSpace;
	float4 ToCascadeOffsetX;
	float4 ToCascadeOffsetY;
	float4 ToCascadeScale;
}

cbuffer SkyBoxInfo : register(b5)
{
	float4 SColor;
}

struct Input
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
	float3 camvector : CPS;
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

float NDFBlinn(float Roughness, float NdotH)
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float n = 2 / m2 - 2;
	return (n + 2) / (2 * PI) * pow(max(abs(NdotH), 0.000001f), n);
}

float NDFGGX(float Roughness, float NdotH)
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float d = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (PI * d * d);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GAFSmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

float3 FresnelSchlick(float HdotV, float3 fresnel)
{
	float t = pow(1.0 - HdotV, 5.0);
	return fresnel + (1.0f - fresnel) * t;
	//return saturate(50.0 * fresnel.g) * t + (1 - t) * fresnel;
}

float3 FresnelSchlickRoughness(float NdotV, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - NdotV, 5.0);
}

float OrenNayer(float NdotV, float NdotL, float roughness)
{
	float rim = 1 - NdotV / 2;
	float fakey = pow(1 - NdotL * rim, 2);
	fakey = 0.62f * (1.0f - fakey);
	return lerp(NdotL, fakey, roughness);
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
	if (diffuseMapFirst.w < 0.9f)
		discard;

	float3 diffuseMap = diffuseMapFirst.xyz;
	bool gammaCorrected = false;
	if (SData2.z && mapMode & 1)
	{
		diffuseMap = pow(diffuseMap, 2.2f);
		gammaCorrected = true;
	}

	float3 normalMap = GetMaterialNormalMap(materialIndex, input.tex, mapMode).xyz;
	float3 specularMap = GetMaterialSpecularMap(materialIndex, input.tex, mapMode).xyz;

	float3 Diffuse = MDiffuse[materialIndex].xyz * diffuseMap;
	float3 Specular = MSpecular[materialIndex].xyz * specularMap;
	float3 Fresnel = MFresnel[materialIndex].xyz;
	float sharpness = MShiness[materialIndex / 4][materialIndex % 4];
	float roughness = MRoughness[materialIndex / 4][materialIndex % 4];
	float metalic = MMetalic[materialIndex / 4][materialIndex % 4];

	if (SData1.x == 0) // Not calculate lihgting just return diffuse
	{
		return float4(Diffuse, 1.0f);
	}

	if (mapMode & 2)
	{
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
		input.normal = normalize(input.normal);
	}

	//Step 2. Calc Light power
	float LightAttenuation = 1.0f; // no light decrease(if LType0 - directional)
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
	float3 LightColor = LColor.xyz;
	LightVector = normalize(LightVector);

	float LightPower = LightAttenuation * LIntensity;

	//Step 3. Calc specular, diffuse factor
	float3 L = LightVector;
	float3 V = input.camvector;
	float3 H = normalize(V + L);
	float3 N = input.normal;

	float NdotL = max(dot(L, N), 0.0f);
	float NdotH = max(dot(H, N), 0.0f);
	float NdotV = max(dot(V, N), 0.0f);
	float HdotV = max(dot(H, V), 0.0f);

	//-Diffuse
	float df = 0.0f;
	if (SData1.z == 0)
	{
		df = NdotL;
	}
	if (SData1.z == 1) //half lambert
	{
		float adjusted = NdotL * 0.5f + 0.5f;
		df = pow(adjusted, SData2.x);
	}
	if (SData1.z == 2) //Oren-Nayer
	{
		df = OrenNayer(NdotV, NdotL, roughness);
	}

	//-Specular
	float specPower = 0.0f;
	float sf = 0.0f;
	if (SData1.w == 0) //phong
	{
		float3 specularVector = normalize(reflect(-L, N));
		specPower = max(dot(specularVector, V), 0.0f);
		sf = pow(specPower, sharpness);
	}
	if (SData1.w == 1) //blinn-phong
	{
		specPower = max(NdotH, 0.0f);
		sf = pow(specPower, sharpness);
	}
	if (SData1.w == 2) //Cook-torrance
	{
		float3 F0 = 0.04f;
		F0 = lerp(F0, Fresnel, metalic);

		float3 Lo = 0.0f;
		{
			float NDF = NDFGGX(roughness, NdotH);
			float G = GAFSmith(NdotV, NdotL, roughness);
			float3 F = FresnelSchlick(HdotV, F0);

			float3 num = NDF * G * F;
			float denum = 4.0 * NdotV * NdotL;

			Specular = num / max(denum, 0.001f);
			float3 kS = F;
			float3 kD = 1.0f - kS;
			kD *= 1.0f - metalic;

			Lo = float3((kD * Diffuse / PI + Specular) * LightPower * NdotL);
		}

		float3 F = FresnelSchlickRoughness(NdotV, F0, roughness);

		float3 KD = 1.0f - F;
		KD *= 1.0f - metalic;

		float3 R = reflect(V, N);
		float3 PrefilteredColor = EnvironmentMap.Sample(SampleType, R, roughness * 11).xyz;
		PrefilteredColor *= SColor; // Sky color
		float3 Specular_ = PrefilteredColor * F;

		float3 Ambient_ = KD * Diffuse * df * LightColor + Specular_;

		float3 color = Ambient_ + Lo;

		float ShadowAtt = 1.0f;
		if (SData1.y == 1)
		{
			if (LType == 0) ShadowAtt = saturate(0.3 + CalcDirShadow(input.worldPosition));
			if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(input.lightToPos, input.position.w));
			if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(input.worldPosition));
		}
		color *= ShadowAtt;
		if (gammaCorrected)
			color = pow(color, 0.4545f);

		return float4(color, 1.0f);
	}

	//step4. Calculate shadow attenuation
	float ShadowAtt = 1.0f;
	if (SData1.y == 1)
	{
		if (LType == 0) ShadowAtt = saturate(0.3 + CalcDirShadow(input.worldPosition));
		if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(input.lightToPos, input.position.w));
		if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(input.worldPosition));
	}

	//step5. Calculate final color
	float3 finalAmbient = input.globalAmbient * MAmbient[materialIndex] * LightPower;
	float3 finalDiffuse = df * Diffuse * LightPower * LightColor;
	float3 finalSpecular = sf * Specular * LightPower * LightColor;
	
	float3 color = finalDiffuse + finalSpecular + finalAmbient;
	color *= ShadowAtt;

	if (gammaCorrected)
	{
		color = pow(color, 0.4545f);
	}

	return float4(color, 1.0f);
}