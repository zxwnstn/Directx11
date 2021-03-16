
#define MaxPart 12

Texture2D ShadowMap : register(t0);
Texture2D materialTexture[MaxPart * 3] : register(t1);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer Light : register(b1)
{
	float4 Position;
	float4 LDirection;
	float4 LColor;
	float  LIntensity;
	int    LType;	//0 Directional, 1 Point, 2 Spot
	float  LInnerAng;
	float  LOuterAngRcp;
	float  LRangeRcp;
	int3   LPadding;
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

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;

	float3 lightToPos : LTP;
	float3 globalAmbient : AMBIENT;

	bool UseShadowMap : SHADOWMAP;
};


float4 GetMaterialDiffuseMap(int index, float2 tex, int mapMode)
{
	float4 diffuseMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 1))
		return diffuseMap;

	if (index == 0) diffuseMap = materialTexture[0].Sample(SampleType,  tex);
	if (index == 1) diffuseMap = materialTexture[3].Sample(SampleType,  tex);
	if (index == 2) diffuseMap = materialTexture[6].Sample(SampleType,  tex);
	if (index == 3) diffuseMap = materialTexture[9].Sample(SampleType,  tex);
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

	if (index == 0) NormalMap = materialTexture[1].Sample(SampleType,  tex);
	if (index == 1) NormalMap = materialTexture[4].Sample(SampleType,  tex);
	if (index == 2) NormalMap = materialTexture[7].Sample(SampleType,  tex);
	if (index == 3) NormalMap = materialTexture[10].Sample(SampleType, tex);
	if (index == 4) NormalMap = materialTexture[13].Sample(SampleType, tex);
	if (index == 5) NormalMap = materialTexture[16].Sample(SampleType, tex);
	if (index == 6) NormalMap = materialTexture[19].Sample(SampleType, tex);
	if (index == 7) NormalMap = materialTexture[22].Sample(SampleType, tex);
	return NormalMap;
}

float4 GetMaterialSpecularMap(int index, float2 tex, int mapMode)
{
	float4 SpecularMap = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (!(mapMode & 4))
		return SpecularMap;

	if (index == 0) SpecularMap = materialTexture[2].Sample(SampleType,  tex);
	if (index == 1) SpecularMap = materialTexture[5].Sample(SampleType,  tex);
	if (index == 2) SpecularMap = materialTexture[8].Sample(SampleType,  tex);
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
	return float4(input.globalAmbient, 1.0f);

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

	//this work only has normal map
	if (mapMode & 2)
	{
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
		input.normal = normalize(input.normal);
	}

	float LightAttenuation = 1.0f; // no light decrease
	float3 LightVector = -LDirection.xyz;
	if (LType == 1)
	{
		LightAttenuation = CalcDistAttenuation(length(input.lightToPos), LRangeRcp);
		LightVector = -input.lightToPos;
	}
	else if (LType == 2)
	{
		LightAttenuation = CalcDistAttenuation(length(input.lightToPos), LRangeRcp)
			* CalcConeAttenuation(input.lightToPos, LightVector, LInnerAng, LOuterAngRcp);
	}
	float3 LightColor = LColor.xyz * LightAttenuation;
	LightVector = normalize(LightVector);

	//Step 2. Calc Halfway Vector
	float3 CamVector = normalize(-input.position.xyz);
	float3 NormalProjection = max(dot(input.normal, LightVector), 0.0f) * input.normal;
	float3 HalfVector = NormalProjection - LightVector;
	float3 SpecularVector = normalize(2 * HalfVector + LightVector);

	//Step3. Caclc diffuse, specular factor 
	float df = max(dot(LightVector, input.normal), 0.0f);								//diffuse factor 	

	//float sf = pow(max(dot(SpecularVector, CamVector), 0.0f), MShiness[materialIndex / 4][materialIndex % 4]);	//specular factor 
	float shiness = MShiness[materialIndex / 4][materialIndex % 4];
	float nn = max(dot(SpecularVector, CamVector), 0.0f);
	float sf = pow(nn, shiness);

	//Step4. Calc finale caculated phong blinn
	float3 finalAmbient = input.globalAmbient * MAmbient[materialIndex];
	float3 finalDiffuse  = df * (Diffuse.xyz * LIntensity * LightColor) + finalAmbient * (Diffuse.xyz * LIntensity * LightColor);
	float3 finalSpecular = sf * (Specular.xyz * LIntensity * LightColor);
	float3 color = finalDiffuse + finalSpecular;

	//step5. calc shadow
	if (input.UseShadowMap)
	{
		float2 projectTexCoord;
		projectTexCoord.x = input.position.x / 1280.0f;
		projectTexCoord.y = input.position.y / 720.0f;
		
		float4 shadow = ShadowMap.Sample(SampleType, projectTexCoord);
		float shadowIntensity = shadow.r + shadow.g + shadow.b;
		if (shadowIntensity >= 2.0f)
		{
			return float4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	return float4(color, 1.0f);
}