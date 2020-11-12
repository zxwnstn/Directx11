#pragma once

struct vec2 { float m[2]{0, }; };
struct vec3 { float m[3]{0, }; };
struct vec4 { float m[4]{0, }; };
struct mat4 { float m[4][4]{0, }; };

struct uvec2 { uint32_t m[2]{0, }; };
struct uvec3 { uint32_t m[3]{0, }; };
struct uvec4 { uint32_t m[4]{0, }; };
struct umat4 { uint32_t m[4][4]{ 0, }; };

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

	uint8_t i = 0;
	void push(float weight, uint32_t index);
};

struct FbxData
{
	ControlPoint* ControlPoints = nullptr;
	uint32_t ControlPointCount;

	Vertex* Vertices = nullptr;
	uint32_t* Indices = nullptr;
	uint32_t VerticesCount;

	struct SkeletalAnimtion* Animation = nullptr;
	struct Skeleton* skeleton;
};


class FBXLoader
{
public:
	FBXLoader(const std::string& directorName);
	~FBXLoader();

public:
	void Extract(const std::string & path, const std::string& file);

private:
	bool isExistCache(const std::string& file);

private:
	void init(const std::string& filename);
	void import(const std::string& filename);
	void extractAll();
	void resetScene();

private:
	vec2 procUV(class FbxMesh* mesh, int index, int controlIndex);
	//void procNormal();

	void getControlPoint(class FbxNode* node);
	void getVertices(FbxNode* node);
	void getSkeleton(FbxNode* node);
	void getAnimation(FbxNode* node);
	//void getMaterial();

	void createHierachy(FbxNode* node, int index, int parent);

private:
	class FbxManager* m_Manager;

	//Temporary
	class FbxImporter* importer = nullptr;
	class FbxScene* scene = nullptr;
	class FbxNode* root = nullptr;

	FbxData data;
	std::string filename;

	bool isCached = false;
	bool isLoaded = false;
};