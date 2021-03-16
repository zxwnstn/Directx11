struct GS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

cbuffer CascadedViewProj : register(b0)
{
	matrix viewProj[3] : packoffset(c0);
};

[maxvertexcount(9)]
void main(triangle float4 InPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> OutStream)
{
	for (int iFace = 0; iFace < 3; iFace++)
	{
		GS_OUTPUT output;

		output.RTIndex = iFace;

		for (int v = 0; v < 3; v++)
		{
			output.Pos = mul(InPos[v], viewProj[iFace]);
			OutStream.Append(output);
		}
		OutStream.RestartStrip();
	}
}