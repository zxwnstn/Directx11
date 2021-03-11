#pragma once

namespace Engine {

	enum class SBufferType
	{
		Read,
		Write,
		ReadWrite
	};

	template<typename T>
	class SBuffer
	{
	public:
		SBuffer(SBufferType type, T* data, uint32_t count)
			: m_Type(type)
		{
			const uint32_t stride = sizeof(T);

			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = stride * count;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.StructureByteStride = stride;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			switch (m_Type)
			{
			case SBufferType::Read: bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; break;
			case SBufferType::Write: bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; break;
			case SBufferType::ReadWrite: bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; break;
			}

			D3D11_SUBRESOURCE_DATA subData;
			subData.pSysMem = data;

			if (data) Dx11Core::Get().Device->CreateBuffer(&bufferDesc, &subData, &m_Buffer);
			else Dx11Core::Get().Device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer);
			
			ASSERT(m_Buffer, "Renderer::Create StructuredBuffer failed");

			if (!(m_Type == SBufferType::Write))
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
				srvDesc.BufferEx.FirstElement = 0;
				srvDesc.BufferEx.Flags = 0;
				srvDesc.BufferEx.NumElements = count;

				Dx11Core::Get().Device->CreateShaderResourceView(m_Buffer, &srvDesc, &m_SRView);
				ASSERT(m_SRView, "Renderer::Create StructuredBuffer failed");			
			}

			if (!(m_Type == SBufferType::Read))
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.Flags = 0;
				uavDesc.Buffer.NumElements = count;

				Dx11Core::Get().Device->CreateUnorderedAccessView(m_Buffer, &uavDesc, &m_UAView);
				ASSERT(m_UAView, "Renderer::Create StructuredBuffer failed");

				bufferDesc.Usage = D3D11_USAGE_STAGING;
				bufferDesc.BindFlags = 0;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

				Dx11Core::Get().Device->CreateBuffer(&bufferDesc, 0, &m_OutputBuffer);
				ASSERT(m_OutputBuffer, "Renderer::Create StructuredBuffer failed");
			}
		}
		
		T* GetData(D3D11_BOX* box = nullptr)
		{
			if (m_Type == SBufferType::Read)
				return nullptr;

			if (box) Dx11Core::Get().Context->CopySubresourceRegion(m_OutputBuffer, 0, 0, 0, 0, m_Buffer, 0, box);
			else Dx11Core::Get().Context->CopyResource(m_OutputBuffer, m_Buffer);
			D3D11_MAPPED_SUBRESOURCE mappedData;
			auto ret = Dx11Core::Get().Context->Map(m_OutputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
			
			if (ret != S_OK)
			{
				auto v = Dx11Core::Get().Device->GetDeviceRemovedReason();
				ASSERT(false, "Map failed");
			}
			
			T* data = reinterpret_cast<T*>(mappedData.pData);
			
			return data;
		}

		void Unmap()
		{
			Dx11Core::Get().Context->Unmap(m_OutputBuffer, 0);
		}

		void Bind(uint32_t slot)
		{
			Dx11Core::Get().Context->CSSetShaderResources(slot, 1, &m_SRView);
		}

		void UnSetTarget()
		{
			ID3D11UnorderedAccessView* null = nullptr;
			Dx11Core::Get().Context->CSSetUnorderedAccessViews(0, 1, &null, nullptr);
		}

		void SetAsTarget(uint32_t slot)
		{
			uint32_t count = 0;
			Dx11Core::Get().Context->CSSetUnorderedAccessViews(slot, 1, &m_UAView, &count);
		}

	private:
		SBufferType m_Type;
		ID3D11Buffer* m_Buffer;
		ID3D11Buffer* m_OutputBuffer;
		ID3D11UnorderedAccessView* m_UAView;
		ID3D11ShaderResourceView* m_SRView;
	};

	struct StreamOut
	{
		vec4 pos;
		vec2 uv;
		vec3 normal;

		void print();
	};

	class SOBuffer
	{
	public:
		SOBuffer();
		void Bind();
		void Unbind();
		StreamOut* GetData();

		ID3D11Buffer* m_OutstreamBuffer;
		ID3D11Buffer* m_OutputBuffer;

	private:
		
	};

}