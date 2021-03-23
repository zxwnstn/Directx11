
#define MaxPart 12
#define PI 3.141592f

Texture2D Depth : register(t0);
Texture2D Diffuse : register(t1);
Texture2D Normal : register(t2);
Texture2D Ambient : register(t3);
Texture2D WorldPosition : register(t4);
Texture2D Specular : register(t5);
Texture2D Misc : register(t6);
Texture2D SpotShadowMap : register(t7);
TextureCube PointShadowMap : register(t8);
Texture2DArray DirShadowMap : register(t9);
TextureCube EnvironmentMap : register(t10);

SamplerState SampleTypeClamp : register(s0);
SamplerComparisonState SampleTypePCF : register(s1);
SamplerState SampleType : register(s2);

cbuffer Environment : register(b0)
{
	matrix WorldMatrix;
	float4 EAmbient;
};

cbuffer Camera : register(b1)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int padding_;
};

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
	int3  LPadding;
};

cbuffer LightCam : register(b3)
{
	matrix LView;
	matrix LProjection;
}

cbuffer Cascaded : register(b4)
{
	matrix ToShadowSpace;
	float4 ToCascadeOffsetX;
	float4 ToCascadeOffsetY;
	float4 ToCascadeScale;
}


cbuffer ShadingData : register(b5)
{
	int4 SData1; //x = Lighting, y = shadow, z = diffuse(lambert, half), w = specular(phong blinn) 
	int4 SData2; //x = lambert Pow factor, y = deffered blend factor, z = gamma correction
}

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

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

float CalcSpotShadow(float4 worldPosition)
{
	worldPosition = mul(worldPosition, LView);
	worldPosition = mul(worldPosition, LProjection);

	float3 uv = worldPosition.xyz / worldPosition.w;
	uv.x = uv.x * 0.5f + 0.5f;
	uv.y = -uv.y * 0.5f + 0.5f;

	if (0.0f < uv.x && uv.x < 1.0f && 0.0f < uv.y && uv.y < 1.0f)
	{
		return SpotShadowMap.SampleCmpLevelZero(SampleTypePCF, uv.xy, uv.z);
	}
	return 1.0f;
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

float4 main(Input input) : SV_TARGET
{
	//step1. Unpack G-Buffer
	float4 DepthSample = Depth.Sample(SampleTypeClamp, input.tex);
	float4 DiffuseSample = Diffuse.Sample(SampleTypeClamp, input.tex);
	float4 NormalSample = Normal.Sample(SampleTypeClamp, input.tex);
	float4 AmbientSample = Ambient.Sample(SampleTypeClamp, input.tex);
	float4 WorldPositionSample = WorldPosition.Sample(SampleTypeClamp, input.tex);
	float4 SpecularSample = Specular.Sample(SampleTypeClamp, input.tex);
	float4 MiscSample = Misc.Sample(SampleTypeClamp, input.tex);

	float depth = DepthSample.x;

	float3 worldPosition = WorldPositionSample.xyz;

	float3 diffuse = DiffuseSample.xyz;
	float3 normal = NormalSample.xyz;
	float3 ambient = AmbientSample.xyz;
	float3 specular = SpecularSample.xyz;
	float sharpness = MiscSample.x * 30.0f;
	float roughness = MiscSample.y;
	float metalic = MiscSample.z;

	if (AmbientSample.w == 0.0f)
	{
		if (SData2.y == 0)
		{
			return DiffuseSample;
		}
		else
		{
			discard;
		}
	}

	if (SData1.x == 0)
		return float4(diffuse, 1.0f);

	//step2. Calc Light intensity
	float lightAttenuation = 1.0f;
	float3 lightVector = -LDirection.xyz;
	float3 posToLight = worldPosition - LPosition.xyz;
	float toLightDist = length(posToLight);
	if (LType == 1) //point
	{
		lightAttenuation = CalcDistAttenuation(toLightDist, LRangeRcp);
		lightVector = -posToLight;
	}
	if (LType == 2) //spot
	{
		lightAttenuation = CalcDistAttenuation(toLightDist, LRangeRcp)
			* CalcConeAttenuation(posToLight, lightVector);
	}
	float LightPower = lightAttenuation * LIntensity;
	float3 LightColor = LColor.xyz;
	lightVector = normalize(lightVector);

	//Step 3. Calc specular, diffuse factor
	float3 cpos = mul(CPosition, WorldMatrix);
	float3 CamVector = normalize(cpos - worldPosition);
	float3 L = lightVector;
	float3 V = CamVector;
	float3 H = normalize(V + L);
	float3 N = normal;

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
		float3 Fresnel = specular;
		float3 F0 = 0.04f;
		F0 = lerp(F0, Fresnel, metalic);

		float3 Lo = 0.0f;
		{
			float NDF = NDFGGX(roughness, NdotH);
			float G = GAFSmith(NdotV, NdotL, roughness);
			float3 F = FresnelSchlick(HdotV, F0);

			float3 num = NDF * G * F;
			float denum = 4.0 * NdotV * NdotL;

			float specular_;
			specular_ = num / max(denum, 0.001f);
			float3 kS = F;
			float3 kD = 1.0f - kS;
			kD *= 1.0f - metalic;

			Lo = float3((kD * diffuse / PI + specular_) * LightColor * LightPower * NdotL);
		}

		float3 F = FresnelSchlickRoughness(NdotV, F0, roughness);

		float3 KD = 1.0f - F;
		KD *= 1.0f - metalic;

		float3 R = reflect(V, N);
		float3 PrefilteredColor = EnvironmentMap.Sample(SampleType, R, roughness * 11).xyz;
		float3 Specular_ = PrefilteredColor * F;

		float3 Ambient_ = KD * diffuse * df + Specular_;

		float3 color = Ambient_ + Lo;

		float ShadowAtt = 1.0f;
		if (SData1.y == 1)
		{
			if (LType == 0) ShadowAtt = saturate(0.3 + CalcDirShadow(worldPosition));
			if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(posToLight, depth));
			if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(WorldPositionSample));
		}
		color *= ShadowAtt;
		if (NormalSample.w != 1.0f)
			color = pow(color, 0.4545f);

		return float4(color, 1.0f);
	}

	//step4. calc shadow attenuation
	float ShadowAtt = 1.0f;
	if (SData1.y)
	{
		if (LType == 0) ShadowAtt = saturate(0.3 + CalcDirShadow(worldPosition));
		if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(posToLight, depth));
		if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(WorldPositionSample));
	}
	
	//step5. Calculate final color
	float3 finalAmbient = EAmbient.xyz * ambient * LightPower;
	float3 finalDiffuse = df * diffuse * LightPower * LightColor;
	float3 finalSpecular = sf * specular * LightPower * LightColor;

	float3 color = finalDiffuse + finalSpecular + finalAmbient;
	color *= ShadowAtt;

	if (NormalSample.w != 1.0f) // mean gamma corrected
		color = pow(color, 0.4545);

	return float4(color, 1.0f);
}

