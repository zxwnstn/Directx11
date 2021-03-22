
Texture2D BlurTarget;
SamplerState SampleType;

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float2 adjacency1 : ADJ1;
	float2 adjacency2 : ADJ2;
	float2 adjacency3 : ADJ3;
	float2 adjacency4 : ADJ4;
	float2 adjacency5 : ADJ5;
	float2 adjacency6 : ADJ6;
	float2 adjacency7 : ADJ7;
	float2 adjacency8 : ADJ8;
	float2 adjacency9 : ADJ9;
};

float4 main(Input input) : SV_TARGET
{
	float weight0 = 1.0f;
	float weight1 = 0.9f;
	float weight2 = 0.55f;
	float weight3 = 0.18f;
	float weight4 = 0.1f;

	float normalization = weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4);
	weight0 = weight0 / normalization;
	weight1 = weight1 / normalization;
	weight2 = weight2 / normalization;
	weight3 = weight3 / normalization;
	weight4 = weight4 / normalization;

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	color += BlurTarget.Sample(SampleType, input.adjacency1) * weight4;
	color += BlurTarget.Sample(SampleType, input.adjacency2) * weight3;
	color += BlurTarget.Sample(SampleType, input.adjacency3) * weight2;
	color += BlurTarget.Sample(SampleType, input.adjacency4) * weight1;
	color += BlurTarget.Sample(SampleType, input.adjacency5) * weight0;
	color += BlurTarget.Sample(SampleType, input.adjacency6) * weight1;
	color += BlurTarget.Sample(SampleType, input.adjacency7) * weight2;
	color += BlurTarget.Sample(SampleType, input.adjacency8) * weight3;
	color += BlurTarget.Sample(SampleType, input.adjacency9) * weight4;

	color.a = 1.0f;

	return color;
}