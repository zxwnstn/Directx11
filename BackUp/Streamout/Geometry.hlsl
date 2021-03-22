struct DS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

struct StreamOutput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};


//struct Output
//{
//	float4 Position : SV_POSITION;
//	float2 UV : TEXCOORD;
//	float3 Normal : NORMAL;
//};

[maxvertexcount(3)]
void main(triangle DS_OUTPUT input[3], inout TriangleStream<StreamOutput> outStream)
{
	StreamOutput output;
	for (int i = 0; i < 3; ++i)
	{
		output.Position = input[i].Position;
		output.UV = input[i].UV;
		output.Normal = input[i].Normal;

		/*output.Position = float4(1.0f, 1.0f, 1.0f, 1.0f);
		output.UV = float2(1.0f, 1.0f);
		output.Normal = float3(1.0f, 1.0f, 1.0f);*/

		outStream.Append(output);
	}
}