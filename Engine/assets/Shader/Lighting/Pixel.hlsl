Texture2D shaderTexture[3]; //diffuse, normal, specular
SamplerState SampleType;

cbuffer Environment : register(b0)
{
	float4 EAmbient;
};

cbuffer Light : register(b1)
{
	float4 LPosition;
	float4 LDirection;
	float4 LColor;
	uint LType;			//0 Directional, 1 Point, 2 Spot
	int3 Lpadding;
};

cbuffer Material : register(b2)
{
	int MMode;			//0 Color, 1 $ has Diffuse 2 $ has Normal 4 $ has specular
	float3 MAmbient;
	float4 MDiffuse;
	float3 MSpecular;
	float3 MFresnel;
	float3 MEmmisive;
	float MRoughness;
	int2 Mpadding;
};

struct Input
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 tintColor : COLOR;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 main(Input input) : SV_TARGET
{
	float4 pixel;
	float shininess = (1.0f - MRoughness) * 255.0f;

	float4 MD = MDiffuse;
	float3 MS = MSpecular;

	//0. Calc Maps
	if (1 & MMode)
	{
		float4 diffuseMap;
		diffuseMap = shaderTexture[0].Sample(SampleType, input.tex);
		MD = MDiffuse * diffuseMap;
	}

	if (2 & MMode)
	{
		float4 normalMap;
		normalMap = shaderTexture[2].Sample(SampleType, input.tex);
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
	}
	input.normal = normalize(input.normal);

	if (4 & MMode)
	{
		float4 specularMap;
		specularMap = shaderTexture[1].Sample(SampleType, input.tex);
		MS = MSpecular * specularMap;
	}

	//1. Get Light Color
	float LightPower = 1.0f;						// no light decrease
	float4 LightColor = LColor * LightPower;

	//2. Calc Light Vector
	float3 LightVector = -LDirection.xyz;				// Light to vertex
	if (LType == 1)
	{
		LightVector = (input.position - LPosition).xyz;
	}
	LightVector = normalize(LightVector);

	//3. Calc Halfway Vector
	float3 pos = -input.position.xyz;
	float3 CamVector = normalize(pos);
	float3 HalfVector = CamVector - LightVector;

	////4. Caclc diffuse, specular factor 
	float kd = max(dot(LightVector, input.normal), 0.0f);					//diffuse factor 
	float ks = pow(max(dot(input.normal, HalfVector), 0.0f), shininess);	//specular factor 

	float3 Ambient = EAmbient.xyz * MAmbient;
	float3 Diffuse = kd * (MD * LightColor);
	float3 Specular = ks * (MS * LightColor);

	float3 color = Ambient + Diffuse + Specular + MEmmisive;
	color.x = Ambient.x + Diffuse.x + Specular.x;
	color.y = Ambient.y + Diffuse.y + Specular.y;
	color.z = Ambient.z + Diffuse.z + Specular.z;

	pixel = float4(color, 1.0f);
	return pixel;
}