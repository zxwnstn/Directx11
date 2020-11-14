#pragma once

#include "InputLayout.h"

namespace Engine {

	class ModelBuffer
	{
	private:

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

		ModelBuffer(const InputLayout& inputLayout);

	public:
		ModelBuffer(const ModelBuffer& other) = default;
		void Bind() const;
		inline uint32_t GetIndexCount() const { return Index.Count; }

	private:
		VertexBuffer Vertex;
		IndexBuffer Index;
		InputLayout Layout;

		friend struct BufferBuilder;
	};

	struct BufferBuilder
	{
	public:
		BufferBuilder(const InputLayout& intpuLayout);

		BufferBuilder& SetVertex(void* vertices, uint32_t size, bool isDynamic = false);
		BufferBuilder& SetIndex(void* indices, uint32_t count);
		BufferBuilder& SetBuffer(void* vertices, void* indices, uint32_t count, bool isDynamic = false);
		inline operator std::shared_ptr<ModelBuffer>() const { return std::make_shared<ModelBuffer>(buffer); }

	private:
		ModelBuffer buffer;
	};

}
