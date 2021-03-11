
#define MaxPart 12

Texture2D Depth : register(t0);
Texture2D Diffuse : register(t1);
Texture2D Normal : register(t2);
Texture2D Ambient : register(t3);
Texture2D WorldPosition : register(t4);
Texture2D Misc : register(t5);
Texture2D SpotShadowMap : register(t6);
TextureCube PointShadowMap : register(t7);

SamplerState SampleTypeClamp : register(s0);
SamplerComparisonState SampleTypePCF : register(s1);
SamplerState SampleType : register(s2);

cbuffer Camera : register(b0)
{
	matrix CView;
	matrix CProjection;
	float3 CPosition;
	int padding_;
};

cbuffer Light : register(b1)
{
	float4 LPosition;
	float4 LDirection;
	float4 LColor;
	float  LIntensity;
	int    LType;	//0 Directional, 1 Point, 2 Spot
	float  LInnerAng;
	float  LOuterAngRcp;
	float  LRangeRcp;
	float  LPadding[3];
};

cbuffer LightCam : register(b2)
{
	matrix LView;
	matrix LProjection;
}

cbuffer Gamma : register(b3)
{
	uint4 GammaCorection;
};

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

float CalcConeAttenuation(float3 lightPos, float3 lightDir, float InnerAng, float OuterAngRcp)
{
	float attenuation;

	float3 LightTo = -lightPos;
	float3 LVnormal = normalize(lightDir);
	float3 LTnormal = normalize(LightTo);
	float cos = dot(LVnormal, LTnormal);
	cos = acos(cos);

	attenuation = 1.0f - saturate(cos - InnerAng) * OuterAngRcp;
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
	diffuse = pow(diffuse, 2.2f);
	float3 normal = NormalSample.xyz;

	float3 ambient = AmbientSample.xyz;
	float specular = MiscSample.x;
	float shiness = MiscSample.y;
	float3 worldPosition = WorldPositionSample.xyz;

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
			* CalcConeAttenuation(posToLight, lightVector, LInnerAng, LOuterAngRcp);
	}
	float3 LightColor = LColor.xyz * lightAttenuation;
	lightVector = normalize(lightVector);

	//Diffuse, Specular factor
	float3 CamVector = normalize(CPosition - worldPosition);
	float3 NormalProjection = max(dot(normal, lightVector), 0.0f) * normal;
	float3 HalfVector = NormalProjection - lightVector;
	float3 SpecularVector = normalize(2 * HalfVector + lightVector);

	float df = max(dot(lightVector, normal), 0.0f);	//diffuse factor 	
	float specCos = max(dot(SpecularVector, CamVector), 0.0f);
	float sf = pow(specCos, shiness);

	float ShadowAtt = 1.0f;
	//if (LType == 1) ShadowAtt = saturate(0.3 + CalcPointShadow(posToLight, depth));
	//if (LType == 2) ShadowAtt = saturate(0.3 + CalcSpotShadow(WorldPositionSample));
	
	float3 finalDiffuse = (float3(df, df, df) + ambient) * (diffuse * ShadowAtt * LIntensity * LightColor);
	float3 finalSpecular = sf * (specular * ShadowAtt * LIntensity * LightColor);
	float3 color = finalDiffuse + finalSpecular;
	
	return pow(float4(color, 1.0f), 0.4545);
}

//float CalcShadow(float2 tex, int divide)
//{
//	float2 closer;
//
//	float width = 1.0f / 1280.0f;
//	float heigt = 1.0f / 720.0f;
//	float totalSample = (float)divide * divide;
//	float count = 0.0f;
//
//	for (int i = 0; i < divide; ++i)
//	{
//		for (int j = 0; j < divide; ++j)
//		{
//			float2 closer;
//			closer.x = tex.x + (i - divide / 2) * width;
//			closer.y = tex.y + (j - divide / 2) * heigt;
//
//			float4 PositionSample = WorldPosition.Sample(SampleTypeClamp, closer);
//
//			PositionSample = mul(PositionSample, LView);
//			PositionSample = mul(PositionSample, LProjection);
//
//			float3 uv = PositionSample.xyz / PositionSample.w;
//			uv.x = uv.x * 0.5f + 0.5f;
//			uv.y = -uv.y * 0.5f + 0.5f;
//
//			if (0.0f < uv.x && uv.x < 1.0f && 0.0f < uv.y && uv.y < 1.0f)
//			{
//				if (SpotShadowMap.Sample(SampleTypeClamp, uv.xy).r < uv.z)
//					count += 1.0f;
//			}
//		}
//	}
//	return 1.0f - count / totalSample;
//}