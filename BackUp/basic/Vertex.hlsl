struct Input
{
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;
	float3 f3Normal     : NORMAL;

	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;
};

struct HS_Input
{
	float3 f3Position   : POSITION;
};

//struct HS_Input
//{
//	float4 f3Position : SV_POSITION;
//};

HS_Input main(Input I)
{
	HS_Input O;

	//O.f3Position = float4(I.f3Position, 1.0f);
	O.f3Position = I.f3Position;

	return O;
}
