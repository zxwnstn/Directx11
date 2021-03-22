
cbuffer CubeCamera : register(b0)
{
	matrix LView[6];
	matrix LProjection;
};

struct Input
{
	float4 position : SV_POSITION;
};

struct Output
{
	float4 position : SV_POSITION;
	uint index : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void main(triangle Input input[3], inout TriangleStream<Output> outStream)
{
	for (int surface = 0; surface < 6; ++surface)
	{
		Output output;
		output.index = surface;
		for (int i = 0; i < 3; ++i)
		{
			output.position = mul(input[i].position, LView[surface]);
			output.position = mul(output.position, LProjection);
			outStream.Append(output);
		}
		outStream.RestartStrip();
	}
}