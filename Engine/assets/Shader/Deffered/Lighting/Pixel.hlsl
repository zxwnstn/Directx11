
#define MaxPart 12

Texture2D Depth : register(t0);
Texture2D Diffuse : register(t1);
Texture2D Normal : register(t2);
Texture2D Ambient : register(t3);
Texture2D WorldPosition : register(t4);
Texture2D Misc : register(t5);

SamplerState SampleTypeClamp : register(s0);

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
	int    LPadding[3];
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

	float3 finalDiffuse = (float3(df, df, df) + ambient) * (diffuse * LIntensity * LightColor);
	float3 finalSpecular = sf * (specular * LIntensity * LightColor);
	float3 color = finalDiffuse + finalSpecular;

	return float4(color, 1.0f);
}