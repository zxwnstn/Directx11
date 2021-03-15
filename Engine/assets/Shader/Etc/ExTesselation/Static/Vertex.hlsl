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
	float2 f2TexCoord   : TEXCOORD;

	float3 f3Normal     : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;


};

HS_Input main(Input input)
{
	HS_Input O;

	O.f3Position = input.f3Position;
	O.f3Normal = input.f3Normal;
	O.binormal = input.binormal;
	O.tangent = input.tangent;

	O.f2TexCoord = input.f2TexCoord;
	O.MaterialIndex = input.MaterialIndex;

	return O;
}
