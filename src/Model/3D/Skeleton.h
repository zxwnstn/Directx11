#pragma once

struct Joint
{
	int Parent;
	std::string Name;
	DirectX::XMMATRIX Offset;
};

struct Skeleton 
{
	std::string Name;
	std::vector<Joint> Joints;
	uint32_t NumJoint;

	void* Vertices;
	void* Indices;
	uint32_t VerticesCount;
};

class SkeletonArchive 
{
public:
	static void Add(Skeleton* skeleton);
	static bool Has(const std::string& name);
	Skeleton* Get(const std::string& name);
};
