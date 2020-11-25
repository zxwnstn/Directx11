
#define MaxPart 12

Texture2D materialTexture[MaxPart * 3];
SamplerState SampleType;

cbuffer Light : register(b1)
{
	float4 LPosition;
	float4 LDirection;
	float4 LColor;
	float  LIntensity;
	int    LType;	//0 Directional, 1 Point, 2 Spot
	int    padding[2];
};

cbuffer Materials : register(b2)
{
	float4 MAmbient[MaxPart];
	float4 MDiffuse[MaxPart];
	float4 MSpecular[MaxPart];
	float4 MEmmisive[MaxPart];
	float4 MFresnel[MaxPart];
	float4 MShiness[MaxPart / 4];
	
	//1 $ has Diffuse 2 $ has Normal 4 $ has specular
	int4  MMode[MaxPart / 4];			
};

struct Input
{
	float3 globalAmbient : AMBIENT;

	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	int MaterialIndex : MATERIALIDX;
};

float4 GetMaterialDiffuseMap(int index, float2 tex, int mapMode)
{
	float4 diffuseMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 1))
		return diffuseMap;

	if (index == 0) diffuseMap = materialTexture[0].Sample(SampleType,  tex);
	if (index == 1) diffuseMap = materialTexture[3].Sample(SampleType,  tex);
	if (index == 2) diffuseMap = materialTexture[6].Sample(SampleType,  tex);
	if (index == 3) diffuseMap = materialTexture[9].Sample(SampleType,  tex);
	if (index == 4) diffuseMap = materialTexture[12].Sample(SampleType, tex);
	if (index == 5) diffuseMap = materialTexture[15].Sample(SampleType, tex);
	if (index == 6) diffuseMap = materialTexture[18].Sample(SampleType, tex);
	if (index == 7) diffuseMap = materialTexture[21].Sample(SampleType, tex);
	return diffuseMap;
}

float4 GetMaterialNormalMap(int index, float2 tex, int mapMode)
{
	float4 NormalMap = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (!(mapMode & 2))
		return NormalMap;

	if (index == 0) NormalMap = materialTexture[1].Sample(SampleType,  tex);
	if (index == 1) NormalMap = materialTexture[4].Sample(SampleType,  tex);
	if (index == 2) NormalMap = materialTexture[7].Sample(SampleType,  tex);
	if (index == 3) NormalMap = materialTexture[10].Sample(SampleType, tex);
	if (index == 4) NormalMap = materialTexture[13].Sample(SampleType, tex);
	if (index == 5) NormalMap = materialTexture[16].Sample(SampleType, tex);
	if (index == 6) NormalMap = materialTexture[19].Sample(SampleType, tex);
	if (index == 7) NormalMap = materialTexture[22].Sample(SampleType, tex);
	return NormalMap;
}

float4 GetMaterialSpecularMap(int index, float2 tex, int mapMode)
{
	float4 SpecularMap = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (!(mapMode & 4))
		return SpecularMap;

	if (index == 0) SpecularMap = materialTexture[2].Sample(SampleType,  tex);
	if (index == 1) SpecularMap = materialTexture[5].Sample(SampleType,  tex);
	if (index == 2) SpecularMap = materialTexture[8].Sample(SampleType,  tex);
	if (index == 3) SpecularMap = materialTexture[11].Sample(SampleType, tex);
	if (index == 4) SpecularMap = materialTexture[14].Sample(SampleType, tex);
	if (index == 5) SpecularMap = materialTexture[17].Sample(SampleType, tex);
	if (index == 6) SpecularMap = materialTexture[20].Sample(SampleType, tex);
	if (index == 7) SpecularMap = materialTexture[23].Sample(SampleType, tex);
	return SpecularMap;
}

float4 main(Input input) : SV_TARGET
{
	int materialIndex = input.MaterialIndex;
	int mapMode = MMode[materialIndex / 4][materialIndex % 4];
	mapMode = 5;

	//step 1. Get material mapping color
	float4 diffuseMapFirst  = GetMaterialDiffuseMap(materialIndex, input.tex, mapMode);
	if (diffuseMapFirst.w < 0.9f) discard;
	float3 diffuseMap = diffuseMapFirst.xyz;
	float3 normalMap   = GetMaterialNormalMap(materialIndex, input.tex, mapMode).xyz;
	float3 specularMap = GetMaterialSpecularMap(materialIndex, input.tex, mapMode).xyz;

	float3 Diffuse  = MDiffuse[materialIndex].xyz * diffuseMap;
	float3 Specular = MSpecular[materialIndex].xyz * specularMap;

	//this work only has normal map
	if (mapMode & 2)
	{
		normalMap = normalMap * 2.0f - 1.0f;
		input.normal = normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal;
		input.normal = normalize(input.normal);
	}

	//Step 2. Calc Halfway Vector
	float LightAttenuation = 1.0f; // no light decrease
	float3 LightColor = LColor.xyz * LightAttenuation;
	
	float3 LightVector = -LDirection.xyz;
	if (LType == 1)
		LightVector = input.position.xyz - LPosition; // Light type 1, SpotLight has posistion no direction!
	LightVector = normalize(LightVector);

	float3 CamVector = normalize(-input.position.xyz);

	float3 NormalProjection = max(dot(input.normal, LightVector), 0.0f) * input.normal;
	float3 HalfVector = NormalProjection - LightVector;
	float3 SpecularVector = normalize(2 * HalfVector + LightVector);

	//Step3. Caclc diffuse, specular factor 
	float df = max(dot(LightVector, input.normal), 0.0f);								//diffuse factor 	

	//float sf = pow(max(dot(SpecularVector, CamVector), 0.0f), MShiness[materialIndex / 4][materialIndex % 4]);	//specular factor 
	float shiness = MShiness[materialIndex / 4][materialIndex % 4];
	float nn = max(dot(SpecularVector, CamVector), 0.0f);
	float sf = pow(nn, shiness);

	//Step4. Calc finale caculated phong blinn
	float3 finalAmbient  = input.globalAmbient * MAmbient[materialIndex];
	float3 finalDiffuse  = df * (Diffuse.xyz * LIntensity * LightColor) + +finalAmbient * (Diffuse.xyz * LIntensity * LightColor);
	float3 finalSpecular = sf * (Specular.xyz * LIntensity * LightColor);
	float3 color = finalDiffuse + finalSpecular;

	return float4(color, 1.0f);
}