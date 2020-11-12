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
};

class SkeletonArchive 
{
public:
	static void Add(const std::string& name);
	static bool Has(const std::string& name);
	static std::shared_ptr<Skeleton> Get(const std::string& name);
};
