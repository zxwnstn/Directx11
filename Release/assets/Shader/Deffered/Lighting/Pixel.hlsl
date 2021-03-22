
#define MaxPart 12

Texture2D Depth : register(t0);
Texture2D Diffuse : register(t1);
Texture2D Normal : register(t2);
Texture2D Ambient : register(t3);
Texture2D WorldPosition : register(t4);
Texture2D Misc : register(t5);
Texture2D SpotShadowMap : register(t6);
TextureCube PointShadowMap : register(t7);
Texture2DArray DirShadowMap : register(t8);

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


float4 main(Input input) : SV_TARGET
{
	//step1. Unpack G-Buffer
	float4 DepthSample = Depth.Sample(SampleTypeClamp, input.tex);
	float4 DiffuseSample = Diffuse.Sample(SampleTypeClamp, input.tex);
	float4 NormalSample = Normal.Sample(SampleTypeClamp, input.tex);
	float4 AmbientSample = Ambient.Sample(SampleTypeClamp, input.tex);
	float4 WorldPositionSample = WorldPosition.Sample(SampleTypeClamp, input.tex);
	float4 MiscSample = Misc.Sample(SampleTypeClamp, input.tex);

	float depth = DepthSample.x;
	float3 diffuse = DiffuseSample.xyz;

	float3 normal = NormalSample.xyz;
	float3 ambient = AmbientSample.xyz;
	float3 specular = MiscSample.xyz;
	float sharpeness = MiscSample.w * 30.0f;
	float3 worldPosition = WorldPositionSample.xyz;

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
	//-Diffuse
	float df = max(dot(lightVector, normal), 0.0f);	//diffuse factor 	
	if (SData1.z == 1) //half lambert
	{
		df = df * 0.5f + 0.5f;
		df = pow(df, SData2.x);
	}

	//-Specular
	float3 cpos = mul(CPosition, WorldMatrix);
	float3 CamVector = normalize(cpos - worldPosition);
	float specPower = 0.0f;
	if (SData1.w == 0) //phong
	{
		float3 specularVector = normalize(reflect(-lightVector, normal));
		specPower = max(dot(specularVector, CamVector), 0.0f);
	}
	if (SData1.w == 1)//blinn-phong
	{
		float3 specularVector = normalize(CamVector + lightVector);
		specPower = max(dot(specularVector, normal), 0.0f);
	}
	float sf = pow(specPower, sharpeness);

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

