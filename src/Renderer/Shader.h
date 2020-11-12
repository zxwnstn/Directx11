#pragma once

#include "ModelBuffer.h"
#include "Common/Camera.h"

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

public:
	Shader(const std::string& path);
	Shader() = default;

public:

	void Bind() const;

	BufferBuilder CreateCompotibleBuffer();

	template<typename BufferType>
	void SetParam(const BufferType& data)
	{
		CBuffer::Type type = CBuffer::GetType<BufferType>::value;

		auto find = CBuffers.find(type);
		if (find == CBuffers.end()) return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Dx11Core::Get().Context->Map(find->second, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		BufferType* mappedData = (BufferType*)mappedResource.pData;
		mappedData->Copy(data);

		Dx11Core::Get().Context->Unmap(find->second, 0);

		unsigned bufferNumber = 0;
		Dx11Core::Get().Context->VSSetConstantBuffers(bufferNumber, 1, &find->second);
	}
	void SetCameraParam(const CBuffer::Camera& data);
	void SetBoneParam(DirectX::XMFLOAT4X4* skinnedTransform, uint32_t count);
	void SetTransformParam(const Transform& data);
	void SetCamParam(const Camera& data);

private:
	void AddCBuffer(const std::filesystem::path& path);
	void CreateCBuffer(CBuffer::Type cbtype);
	void CreateSampler();
	void SetLayout(const std::filesystem::path& path, ID3D10Blob* binary);

	void CreateShader(ID3D10Blob* binary, Type type);
	ID3D10Blob* CompileShader(const std::filesystem::path& path, Type type);

private:
	std::string Path;
	uint8_t TypeKey = 0;
	InputLayout InputLayout;

	std::unordered_map<Type, ShaderVar> Shaders;
	std::unordered_map<CBuffer::Type, ID3D11Buffer*> CBuffers;

	ID3D11SamplerState* SamplerState = nullptr;
};

class ComputeShader
{

};