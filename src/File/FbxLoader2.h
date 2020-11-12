#pragma once

#include "Resource/SkeletalAnimation.h"

struct vec2 { float m[2]; };
struct vec3 { float m[3]; };
struct vec4 { float m[4]; };
struct mat4 { float m[4][4]; };

struct uvec2 { uint32_t m[2]; };
struct uvec3 { uint32_t m[3]; };
struct uvec4 { uint32_t m[4]; };
struct umat4 { uint32_t m[4][4]; };

struct Vertex
{
	vec3 Position;
	vec2 UV;
	vec3 Normal;
	vec3 BiNormal;
	vec3 BiTangent;

	vec4 BoneWeight;
	uvec4 BoneIndex;
};

struct ControlPoint
{
	vec3 Position;
	vec4 BoneWeight;
	uvec4 BoneIndex;
};

struct FbxData
{
	ControlPoint* ControlPoints = nullptr;
	Vertex* Vertices;
	uint32_t* Indices;
	uint32_t VerticesCount;

	AnimationClip* animClip;
};



class FBXLoader
{
public:
	enum class ExtractInform : uint64_t
	{
		Position	 =   BIT(1), 
		UV			 =   BIT(2),
		Normal		 =   BIT(3),
		BiNormal	 =   BIT(4),
		BiTangent	 =   BIT(5),
		ControlPoint =   BIT(6),
		Material	 =   BIT(7),
		Animation	 =   BIT(8)
	};

public:
	FBXLoader(const std::string& file);

public:
	void Extract(uint64_t flag);

private:
	bool isReady();
	bool isExistCache(const std::string& file);

private:
	vec2 procUV(class FbxMesh* mesh, int index, int controlIndex);
	//void procNormal();

	void getControlPoint(FbxNode* node);
	void getVertices(FbxNode* node);
	//void getMaterial();
	void getBoneHierachy(FbxNode* node);
	void getAnimation(FbxNode* node);

	void createHierachy(FbxNode* node, int index, int parent);

private:
	class FbxManager* m_Manager;

	//Temporary
	class FbxImporter* importer = nullptr;
	class FbxScene* scene = nullptr;
	class FbxNode* root = nullptr;

	FbxData data;

	bool isCached = false;
};