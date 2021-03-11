struct Input
{
	float3 f3Position   : POSITION;
	float2 f2TexCoord   : TEXCOORD;
	float3 f3Normal     : NORMAL;

	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;

	int MaterialIndex : MATERIALIDX;

	float4 boneWeight  : WEIGHT;
	float4 boneWeightr : WEIGHTR;
	uint4 boneIndices  : BONEINDICES;
	uint4 boneIndicesr : BONEINDICESR;
};

struct HS_Input
{
	float3 f3Position   : POSITION;
	float3 f3Normal     : NORMAL;
	float2 f2TexCoord   : TEXCOORD;
};

HS_Input main(Input I)
{
	HS_Input O;

	O.f3Position = I.f3Position;
	O.f3Normal = I.f3Normal;
	O.f2TexCoord = I.f2TexCoord;

	return O;
}
