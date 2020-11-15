#pragma once

#include "ModelBuffer.h"
#include "Common/Camera.h"
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

	public:
		Shader(const std::string& path);
		Shader() = default;

	public:

		void Bind() const;

		BufferBuilder CreateCompotibleBuffer();

		void SetCameraParam(Camera& data);
		void SetBoneParam(DirectX::XMFLOAT4X4* skinnedTransform, uint32_t count);
		void SetTransformParam(const Transform& data);

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

}
