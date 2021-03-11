#pragma once

#include "Common/Mesh.h"

namespace Engine {

	struct BufferBuilder;

	class ModelBuffer
	{
	private:
		ModelBuffer(MeshType type);
		struct VertexBuffer
		{
			void Init(void* vertices, uint32_t size, bool isDynamic);
			void SetData(float* data, uint32_t size);

			ID3D11Buffer* Buffer = nullptr;
			bool IsDynamic;
		};

		struct IndexBuffer
		{
			void Init(void* indices, uint32_t count);

			ID3D11Buffer* Buffer = nullptr;
			uint32_t Count;
		};

	public:
		static BufferBuilder Create(MeshType type);

	public:
		void Bind() const;
		inline uint32_t GetIndexCount() const { return Index.Count; }
		const MeshType GetMeshType() const { return Type; }

	private:
		VertexBuffer Vertex;
		IndexBuffer Index;
		uint32_t Stride;
		MeshType Type;

		friend struct BufferBuilder;
		friend class Renderer;
	};

	struct BufferBuilder
	{
	public:
		BufferBuilder(MeshType type);

		BufferBuilder& SetMesh(std::shared_ptr<struct SkeletalMesh> mesh, bool isDynamic = false);
		BufferBuilder& SetMesh(std::shared_ptr<struct StaticMesh> mesh, bool isDynamic = false);

		inline operator std::shared_ptr<ModelBuffer>() const { return std::shared_ptr<ModelBuffer>(buffer); }

	private:
		BufferBuilder& SetVertices(void* vertices, uint32_t count);
		BufferBuilder& SetIndices(void* indices, uint32_t count);

	private:
		ModelBuffer* buffer;
		uint32_t stride;

		friend class Renderer;
	};

}
