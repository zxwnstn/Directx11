struct Input
{
	float3 position : POSITION;
	float2 tex      : TEXCOORD;

	float3 normal   : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent  : TANGENT;

	int materialIndex : MATERIALIDX;
};

Input main(Input input)
{
	return input;
}
