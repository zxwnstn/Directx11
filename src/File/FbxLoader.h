#pragma once
#include "Resource/Animation.h"

struct vec2
{
	union {
		struct {
			float x, y;
		};
		float m[2]{ 0.0f, };
	};
};

struct vec3
{
	union {
		struct {
			float x, y, z;
		};
		float m[3]{ 0.0f, };
	};
};

struct vec4
{
	vec4()
	{}

	union {
		struct {
			float x, y, z, w;
		};
		float m[4]{ 0.0f, };
	};
};

struct mat4
{
	mat4()
	{}

	union {
		struct {
			vec4 r1, r2, r3, r4;
		};
		float m[4][4];
	};
};

struct BoneWeight
{
	unsigned int BoneIndex = 0;
	double Weight = 0;
};

struct ControlPoint
{
	vec4 pos;
	std::vector<BoneWeight> BoneWeights;
	std::string boneName;
};

struct Vertex
{
	vec4 pos;
	vec2 uvs;
	float Weight[4];
	uint32_t BoneIndex[4];
	//vec4 color{ 1.0f, 0.0f, 1.0f, 1.0f };
	//vec2 uv;
	//vec3 normal;
	//vec3 binoral;
	//vec3 bitangent;

	bool operator==(const Vertex& other) const
	{
		if (pos.x != other.pos.x || pos.y != other.pos.y || pos.z != other.pos.z)
			return false;

		if (uvs.x != other.uvs.x || uvs.y != other.uvs.y)
			return false;

		return true;
	}
};

struct HashFunction
{
	size_t operator()(const Vertex& pos) const
	{
		size_t res = 17;
		res = res * 31 + std::hash<float>()(pos.pos.x);
		res = res * 31 + std::hash<float>()(pos.pos.y);
		res = res * 31 + std::hash<float>()(pos.pos.z);
		res = res * 31 + std::hash<float>()(pos.pos.w);
		res = res * 31 + std::hash<float>()(pos.uvs.x);
		res = res * 31 + std::hash<float>()(pos.uvs.y);
		
		return res;
	}
};

struct FbxResult
{
	std::vector<ControlPoint> ControlPoints;
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	Skeleton skeleton;
};

class FBXLoader
{
private:
	FBXLoader();

public:
	static FBXLoader* Get();
	static void Shutdown();

public:
	FbxResult Import(const std::string& file);

private:
	FbxResult process(class FbxNode* root);

	vec2 procUV(class FbxMesh* mesh, int index, int controlIndex);
	//void procNormal();

	void procControlPoint(FbxNode* node, FbxResult& ret);
	void procVertices(FbxNode* node, FbxResult& ret);
	void procMaterial(FbxNode* node, FbxResult& ret);
	void createHierachy(FbxNode* node, FbxResult& ret, int index, int parent);
	void procBoneHierachy(FbxNode* node, FbxResult& ret);
	void procAnimation(FbxNode* node, FbxResult& ret);

private:
	class FbxManager* m_Manager;
	class FbxImporter* m_Importer;
	class FbxScene* scene;

};