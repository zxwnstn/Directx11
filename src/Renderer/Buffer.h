#pragma once

#include "InputLayout.h"

class Buffer 
{
private:
	
	struct VertexBuffer
	{
		void Init(float* vertices, uint32_t size, bool isDynamic);
		void SetData(float* data, uint32_t size);

		ID3D11Buffer* Buffer = nullptr;
		bool IsDynamic;
	};

	struct IndexBuffer
	{
		void Init(uint32_t* indices, uint32_t count);
		
		ID3D11Buffer* Buffer = nullptr;
		uint32_t Count;
	};

	Buffer(const InputLayout& inputLayout);

public:
	Buffer(const Buffer& other) = default;
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

	BufferBuilder& SetVertex(float * vertices, uint32_t size, bool isDynamic = false);
	BufferBuilder& SetIndex(uint32_t* indices, uint32_t count);
	inline operator std::shared_ptr<Buffer>() const { return std::make_shared<Buffer>(buffer); }

private:
	Buffer buffer;
};