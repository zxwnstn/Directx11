#pragma once

namespace Engine {

	enum class MeshType
	{
		Skeletal,
		Static, 
		Sqaure
	};

	
	struct Vertex
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;
			ar & UV;

			ar & Normal;
			ar & BiNormal;
			ar & Tangent;

			ar & MaterialIndex;
		}

		vec3 Position;
		vec2 UV;

		vec3 Normal;
		vec3 BiNormal;
		vec3 Tangent;

		int MaterialIndex = 0;
	};

	struct SkeletalVertex
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;
			ar & UV;

			ar & Normal;
			ar & BiNormal;
			ar & Tangent;

			ar & MaterialIndex;

			ar & BoneWeight;
			ar & BoneWeightr;
			ar & BoneIndex;
			ar & BoneIndexr;
		}

		vec3 Position;
		vec2 UV;

		vec3 Normal;
		vec3 BiNormal;
		vec3 Tangent;

		int MaterialIndex = 0;

		vec4 BoneWeight;
		vec4 BoneWeightr;
		uvec4 BoneIndex;
		uvec4 BoneIndexr;

		bool checkValid();
	};

	uint32_t GetStride(MeshType type);

	struct SkeletalMesh
	{
	private:
		SkeletalMesh() = default;

	public:
		std::vector<SkeletalVertex> Vertices;
		uint32_t* Indices;
		uint32_t IndiceCount;

		const MeshType Type = MeshType::Skeletal;

		friend class MeshArchive;
	};

	struct StaticMesh
	{
	private:
		StaticMesh() = default;

	public:
		std::vector<Vertex> Vertices;
		uint32_t* Indices;
		uint32_t IndiceCount;

		const MeshType Type = MeshType::Static;

		friend class MeshArchive;
	};

	class MeshArchive
	{
	private:
		using SerialIndecies = uint32_t * ;

	public:
		static void Init();
		static void ShutDown();

		static std::shared_ptr<StaticMesh> AddStaticMesh(const std::string& name);
		static bool HasStaticMesh(const std::string& name);
		static std::shared_ptr<StaticMesh> GetStaticMesh(const std::string& name);

		static std::shared_ptr<SkeletalMesh> AddSkeletalMesh(const std::string& name);
		static bool HasSkeletalMesh(const std::string& name);
		static std::shared_ptr<SkeletalMesh> GetSkeletalMesh(const std::string& name);

		static const SerialIndecies GetSerialIndices() { return s_SerialIndecies; }

		static std::vector<std::string> GetStaticMeshList();
		static std::vector<std::string> GetSkeletalMeshList();

	private:
		static SerialIndecies s_SerialIndecies;
		static std::unordered_map<std::string, std::shared_ptr<StaticMesh>> s_StaticMeshes;
		static std::unordered_map<std::string, std::shared_ptr<SkeletalMesh>> s_SkeletalMeshes;
	};


	struct ControlPoint
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Position;

			ar & BoneWeight;
			ar & BoneWeightr;

			ar & BoneIndex;
			ar & BoneIndexr;
		}
		vec3 Position;

		vec4 BoneWeight;
		vec4 BoneWeightr;

		uvec4 BoneIndex;
		uvec4 BoneIndexr;

		int MaterialIndex = 0;
		uint8_t i = 0;

		void push(float weight, uint32_t index);
	};

}
