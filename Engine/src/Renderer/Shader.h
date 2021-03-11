#pragma once

#include "ModelBuffer.h"
#include "CBuffer.h"

namespace Engine {

	struct InputLayout
	{
		uint32_t Stride;
		ID3D11InputLayout* D11Layout;
	};


	class Shader
	{
	public:
		using ShaderVar = std::variant<ID3D11VertexShader*, ID3D11HullShader*, ID3D11DomainShader*, ID3D11GeometryShader*, ID3D11PixelShader*, ID3D11ComputeShader*>;

		enum Type
		{
			VertexShader = 0b000001,
			HullShader = 0b000010,
			DomainShader = 0b000100,
			GeometryShader = 0b001000,
			PixelShader = 0b010000,
			ComputeShader = 0b100000,
			None
		};

		struct ContantBuffer
		{
			int Register;
			Type ShaderType;
			ID3D11Buffer* Buffer;
		};

	private:
		Shader(const std::string& path, const std::string& name);

	public:
		BufferBuilder CreateCompotibleBuffer();

	public:
		void Bind() const;
		void Unbind();
		void Dipatch(uint32_t x, uint32_t y, uint32_t z);
		bool Has(Type type);

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
			case Type::GeometryShader: Dx11Core::Get().Context->GSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			case Type::DomainShader: Dx11Core::Get().Context->DSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			case Type::HullShader: Dx11Core::Get().Context->HSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			case Type::ComputeShader: Dx11Core::Get().Context->CSSetConstantBuffers(bufferNumber, 1, &cbuffer.Buffer); return;
			}
		}

	private:
		void AddCBuffer(const std::filesystem::path& path);
		void CreateCBuffer(int regNumber, Type shaderType, CBuffer::Type cbtype);
		void CreateSampler(const std::filesystem::path& path);
		void SetLayout(const std::filesystem::path& path, ID3D10Blob* binary);
		void CreateShader(ID3D10Blob* binary, Type type);
		void CheckStreamOut(const std::filesystem::path& path);
		ID3D10Blob* CompileShader(const std::filesystem::path& path, Type type);

	private:
		std::string Path;
		std::string Name;
		uint8_t TypeKey = 0;
		InputLayout Layout;

		std::unordered_map<Type, ShaderVar> Shaders;
		std::unordered_map<CBuffer::Type, ContantBuffer> CBuffers;
		std::vector<D3D11_SO_DECLARATION_ENTRY> SOLayout;

		ID3D11SamplerState* SamplerState[10];
		uint32_t SamplerNumber = 0;

		friend class ShaderArchive;
	};

	class ShaderArchive
	{
	public:
		static std::shared_ptr<Shader> Add(const std::string& path, const std::string& name);
		static std::shared_ptr<Shader> Get(const std::string& name);
		static bool Has(const std::string& name);

	private:
		static std::unordered_map<std::string, std::shared_ptr<Shader>> s_Shaders;

		friend class Renderer;
	};

}
