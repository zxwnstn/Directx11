
Texture2D depthMap : register(t0);
Texture2D myTexture : register(t1);

SamplerState SampleType : register(s0);
SamplerState SampleTypeClamp : register(s1);

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;

	bool CreateShadowMap : SHADOWMAP;
	float4 lightViewPosition : LPOSITION;
	float4 depthposition : POS;
	float4 bias : BIAS;
};

float4 main(Input input) : SV_TARGET
{
	if (input.CreateShadowMap)
	{
		float4 diffuse = myTexture.Sample(SampleType, input.tex);
		if (diffuse.a < 0.9f)
			discard;

		float depth = input.depthposition.z / input.depthposition.w;
		return float4(depth, depth, depth, 1.0f);
	}

	float4 onLight = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 onShadow = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float2 projectTexCoord;
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
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