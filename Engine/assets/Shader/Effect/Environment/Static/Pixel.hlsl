
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
	float4 MRoughness[MaxPart / 4];
	float4 MMetalic[MaxPart / 4];

	//1 $ has Diffuse 2 $ has Normal 4 $ has specular
	int4  MMode[MaxPart / 4];
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

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	int MaterialIndex : MATERIALIDX;
	uint index : SV_RenderTargetArrayIndex;
};

float4 main(Input input) : SV_TARGET
{
	int materialIndex = input.MaterialIndex;
	int mapMode = MMode[materialIndex / 4][materialIndex % 4];

	float3 diffuseMap = GetMaterialDiffuseMap(materialIndex, input.tex, mapMode).xyz;
	float3 diffuseMat = MDiffuse[materialIndex].xyz;
	float3 color = diffuseMap * diffuseMat;

	return float4(diffuseMat, 1.0f);
}