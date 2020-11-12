#include "pch.h"

#include "ModelBuffer.h"
#include "Renderer.h"
#include "Dx11Core.h"

void ModelBuffer::VertexBuffer::Init(void * vertices, uint32_t size, bool isDynamic)
{
	IsDynamic = isDynamic;
	D3D11_BUFFER_DESC vertexBufferDesc;
	if (IsDynamic)
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0;
	}
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = size;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	auto ret = Dx11Core::Get().Device->CreateBuffer(&vertexBufferDesc, &vertexData, &Buffer);
}

void ModelBuffer::VertexBuffer::SetData(float * data, uint32_t size)
{
	if (!IsDynamic)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Dx11Core::Get().Context->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto verticesPtr = (float*)mappedResource.pData;
	memcpy(verticesPtr, (void*)data, size);

	Dx11Core::Get().Context->Unmap(Buffer, 0);
}

void ModelBuffer::IndexBuffer::Init(void * indices, uint32_t count)
{
	Count = count;

	D3D11_BUFFER_DESC indexBufferDecs;
	indexBufferDecs.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDecs.ByteWidth = sizeof(uint32_t) * Count;
	indexBufferDecs.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDecs.CPUAccessFlags = 0;
	indexBufferDecs.MiscFlags = 0;
	indexBufferDecs.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	Dx11Core::Get().Device->CreateBuffer(&indexBufferDecs, &indexData, &Buffer);
}

BufferBuilder::BufferBuilder(const InputLayout & intpuLayout)
	: buffer(intpuLayout)
{
}

BufferBuilder & BufferBuilder::SetVertex(void* vertices, uint32_t size, bool isDynamic)
{
	buffer.Vertex.Init(vertices, size, isDynamic);
	return *this;
}

BufferBuilder & BufferBuilder::SetIndex(void* indices, uint32_t count)
{
	buffer.Index.Init(indices, count);
	return *this;
}


ModelBuffer::ModelBuffer(const InputLayout & inputLayout)
	: Layout(inputLayout)
{
}

void ModelBuffer::Bind() const
{
	UINT offset = 0;

	Dx11Core::Get().Context->IASetVertexBuffers(0, 1, &Vertex.Buffer, &Layout.Stride, &offset);
	Dx11Core::Get().Context->IASetIndexBuffer(Index.Buffer, DXGI_FORMAT_R32_UINT, 0);
	Dx11Core::Get().Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

