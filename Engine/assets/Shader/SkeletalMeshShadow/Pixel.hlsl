
#define MaxPart 12

Texture2D depthMap : register(t0);
Texture2D materialTexture[MaxPart * 3] : register(t1);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	int MaterialIndex : MATERIALIDX;

	bool CreateShadowMap : SHADOWMAP;
	float4 lightViewPosition : LPOSITION;
	float4 depthposition : POS;
	float4 bias : BIAS;
};

float4 GetMaterialDiffuseMap(int index, float2 tex, int mapMode)
{
	float4 diffuseMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

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

float4 main(Input input) : SV_TARGET
{
	if (input.CreateShadowMap)
	{
		float4 diffuse = GetMaterialDiffuseMap(input.MaterialIndex, input.tex, 1);
		if (diffuse.a < 0.9f)
			discard;

		float depth = input.depthposition.z / input.depthposition.w;
		return float4(depth, depth, depth, 1.0f);
	}
	
	float4 onLight = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 onShadow = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float2 projectTexCoord;
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
	if (input.bias.y < projectTexCoord.x && projectTexCoord.x < input.bias.z && input.bias.y < projectTexCoord.y && projectTexCoord.y < input.bias.z)
	{
		float bias = 0.0000125f;
		float shadowMapDepth = depthMap.Sample(SampleTypeClamp, projectTexCoord).r;

		float currentDepth = input.lightViewPosition.z / input.lightViewPosition.w - bias;

		if (shadowMapDepth < currentDepth)
			return onShadow;
	}

	return onLight;
}