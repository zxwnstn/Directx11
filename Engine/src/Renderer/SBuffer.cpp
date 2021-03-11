#include "pch.h"

#include "SBuffer.h"
#include "Dx11Core.h"

namespace Engine {

	SOBuffer::SOBuffer()
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = 2048;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		Dx11Core::Get().Device->CreateBuffer(&desc, 0, &m_OutstreamBuffer);
		ASSERT(m_OutstreamBuffer, "Renderer::Create Streamoutput Buffer failed");

		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		Dx11Core::Get().Device->CreateBuffer(&desc, 0, &m_OutputBuffer);
		ASSERT(m_OutputBuffer, "Renderer::Create Output Buffer failed");
	}

	void SOBuffer::Bind()
	{
		Dx11Core::Get().Context->SOSetTargets(1, &m_OutstreamBuffer, 0);
	}

	void SOBuffer::Unbind()
	{
		ID3D11Buffer* null = nullptr;
		Dx11Core::Get().Context->SOSetTargets(1, &null, 0);
	}

	StreamOut* SOBuffer::GetData()
	{
		Dx11Core::Get().Context->CopyResource(m_OutputBuffer, m_OutstreamBuffer);
		
		D3D11_MAPPED_SUBRESOURCE mappedData;
		Dx11Core::Get().Context->Map(m_OutputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
		StreamOut* data = reinterpret_cast<StreamOut*>(mappedData.pData);
		Dx11Core::Get().Context->Unmap(m_OutputBuffer, 0);
		return data;
	}

	void StreamOut::print()
	{
		std::cout << "position : ";
		for (int i = 0; i < 4; ++i)
			std::cout << pos.m[i] << " ";
		std::cout << "\n";
	}

	

}