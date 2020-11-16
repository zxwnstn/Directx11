#pragma once

#include "ModelBuffer.h"
#include "CBuffer.h"

namespace Engine {

	class Shader
	{
	public:
		using ShaderVar = std::variant<ID3D11VertexShader*, ID3D11HullShader*, ID3D11DomainShader*, ID3D11GeometryShader*, ID3D11PixelShader*>;

		enum Type
		{
			VertexShader = 0b00001,
			HullShader = 0b00010,
			DomainShader = 0b00100,
			GeometryShader = 0b01000,
			PixelShader = 0b10000,
			None
		};

		struct ContantBuffer
		{
			int Register;
			Type ShaderType;
			ID3D11Buffer* Buffer;
		};

	public:
		Shader(const std::string& path);
		Shader() = default;
		BufferBuilder CreateCompotibleBuffer();

	public:
		void Bind() const;
		
		template<class ConstantBuffer, class PramType>
		void SetParam(PramType& param)
		{
			CBuffer::Type type = CBuffer::detail::GetType<ConstantBuffer>::value;
			auto find = CBuffers.find(type);
			if (find == CBuffers.end()) return;

			auto& cbuffer = find->second;

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			Dx11Core::Get().Context->Map(cbuffer.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			ConstantBuffer* mappedData = (ConstantBuffer*)mappedResource.pData;
			mappedData->Upload(param);

			Dx11Core::Get().Context->Unmap(cbuffer.Buffer, 0);

			unsigned bufferNumber = cbuffer.Register;
			switch (cbuffer.ShaderType)
			{
			case Type::VertexShader: Dx11Core::Get().Context->VSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			case Type::PixelShader: Dx11Core::Get().Context->PSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			}
		}

	private:
		void AddCBuffer(const std::filesystem::path& path);
		void CreateCBuffer(int regNumber, Type shaderType, CBuffer::Type cbtype);
		void CreateSampler();
		void SetLayout(const std::filesystem::path& path, ID3D10Blob* binary);
		void CreateShader(ID3D10Blob* binary, Type type);
		ID3D10Blob* CompileShader(const std::filesystem::path& path, Type type);

	private:
		std::string Path;
		uint8_t TypeKey = 0;
		InputLayout InputLayout;

		std::unordered_map<Type, ShaderVar> Shaders;
		std::unordered_map<CBuffer::Type, ContantBuffer> CBuffers;

		ID3D11SamplerState* SamplerState = nullptr;
	};

	class ComputeShader
	{

	};

}
