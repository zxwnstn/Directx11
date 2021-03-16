
#define MaxPart 12

Texture2D materialTexture[MaxPart * 3] : register(t0);
SamplerState SampleType : register(s0);

cbuffer Materials : register(b0)
{
	float4 MAmbient[MaxPart];
	float4 MDiffuse[MaxPart];
	float4 MSpecular[MaxPart];
	float4 MEmmisive[MaxPart];
	float4 MFresnel[MaxPart];
	float4 MShiness[MaxPart / 4];
	int4  MMode[MaxPart / 4];
};

struct GBuffer
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal  : SV_TARGET1;
	float4 Ambient : SV_TARGET2;
	float4 WorldPosition : SV_TARGET3;
	float4 Misc : SV_TARGET4;
};

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;

	float3 gAmbient : AMBIENT;
	float3 worldPosition : WORLD_POS;
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

GBuffer main(Input input) : SV_TARGET
{
	GBuffer output;

	int materialIndex = input.MaterialIndex;
	int mapMode = MMode[materialIndex / 4][materialIndex % 4];

	float4 diffuseMap = GetMaterialDiffuseMap(materialIndex, input.tex, mapMode);
	if (diffuseMap.w < 0.9f)
		discard;

	float3 normalMap = GetMaterialNormalMap(materialIndex, input.tex, mapMode).xyz;
	float3 specularMap = GetMaterialSpecularMap(materialIndex, input.tex, mapMode).xyz;

	float3 Diffuse = diffuseMap * MDiffuse[materialIndex].xyz;
	float3 Specular = specularMap * MSpecular[materialIndex].xyz;;
	float3 Ambient = input.gAmbient * MAmbient[materialIndex].xyz;
	if (mapMode & 2)
	{
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
		input.normal = normalize(input.normal);
	}

	output.Diffuse = float4(Diffuse, 1.0f);
	output.Normal = float4(input.normal, 1.0f);
	output.Ambient = float4(Ambient, 1.0f);
	output.WorldPosition = float4(input.worldPosition, 1.0f);
	float shiness = MShiness[materialIndex / 4][materialIndex % 4];
	shiness /= 30.0f;
	output.Misc = float4(Specular, shiness);

	return output;
}