
Texture2D ShadowMap : register(t0);
Texture2D Texture : register(t1);

SamplerState SampleType			: register(s0);

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	bool UseShadowMap : SHADOWMAP;
};

float4 main(Input input) : SV_TARGET
{
	float4 pixel = Texture.Sample(SampleType, input.tex);

	if (input.UseShadowMap)
	{
		float2 projectTexCoord;
		projectTexCoord.x =  input.position.x / 1280.0f;
		projectTexCoord.y =  input.position.y / 720.0f;

		float4 shadow = ShadowMap.Sample(SampleType, projectTexCoord);
		float shadowIntensity = shadow.r + shadow.g + shadow.b;
		if (shadowIntensity < 2.0f)
		{
			return float4(pixel.xyz * 0.4f, 1.0f);
		}
	}

	return pixel;
}