#include "pch.h"

#include "Shader.h"
#include "Dx11Core.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Common/Material.h"

namespace Engine {

	constexpr uint32_t GetCBSize(CBuffer::Type type)
	{
		switch (type)
		{
		case CBuffer::Type::Camera: return sizeof(CBuffer::Camera);
		case CBuffer::Type::Transform: return sizeof(CBuffer::Transform);
		case CBuffer::Type::Light: return sizeof(CBuffer::Light);
		case CBuffer::Type::Bone: return sizeof(CBuffer::Bone);
		case CBuffer::Type::Environment: return sizeof(CBuffer::Environment);
		case CBuffer::Type::Material: return sizeof(CBuffer::Material);
		case CBuffer::Type::Materials: return sizeof(CBuffer::Materials);
		}
		return 0;
	}

	CBuffer::Type GetCBType(const std::string& name)
	{
		if (name == "Camera") return CBuffer::Type::Camera;
		if (name == "Light") return CBuffer::Type::Light;
		if (name == "Transform") return CBuffer::Type::Transform;
		if (name == "Bone") return CBuffer::Type::Bone;
		if (name == "Environment") return CBuffer::Type::Environment;
		if (name == "Material") return CBuffer::Type::Material;
		if (name == "Materials") return CBuffer::Type::Materials;
		return CBuffer::Type::None;
	}

	DXGI_FORMAT GetDxDataFormat(const std::string& dataFormat)
	{
		if (dataFormat == "int") return DXGI_FORMAT_R32_SINT;
		if (dataFormat == "float") return DXGI_FORMAT_R32_FLOAT;
		if (dataFormat == "float2") return DXGI_FORMAT_R32G32_FLOAT;
		if (dataFormat == "float3") return DXGI_FORMAT_R32G32B32_FLOAT;
		if (dataFormat == "float4") return DXGI_FORMAT_R32G32B32A32_FLOAT;
		if (dataFormat == "uint4") return DXGI_FORMAT_R32G32B32A32_UINT;
		return DXGI_FORMAT_UNKNOWN;
	}

	uint32_t GetDxDataSize(const std::string& dataFormat)
	{
		if (dataFormat == "int") return 4;
		if (dataFormat == "float") return 4;
		if (dataFormat == "float2") return 8;
		if (dataFormat == "float3") return 12;
		if (dataFormat == "float4") return 16;
		if (dataFormat == "uint4") return 16;
		return 0;
	}

	Shader::Type GetShaderType(const std::string & filename)
	{
		if (filename == "Pixel.hlsl") return Shader::Type::PixelShader;
		if (filename == "Hull.hlsl") return Shader::Type::HullShader;
		if (filename == "Domain.hlsl") return Shader::Type::DomainShader;
		if (filename == "Geometry.hlsl") return Shader::Type::GeometryShader;
		if (filename == "Vertex.hlsl") return Shader::Type::VertexShader;

		return Shader::Type::None;
	}

	void Shader::CreateCBuffer(int regNumber, Type shaderType, CBuffer::Type cbtype)
	{
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC BufferDesc;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = GetCBSize(cbtype);
		BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = 0;
		Dx11Core::Get().Device->CreateBuffer(&BufferDesc, NULL, &buffer);
		ASSERT(buffer, "Create constant buffer failed");

		CBuffers.emplace(cbtype, Shader::ContantBuffer{regNumber, shaderType, buffer});
	}

	void Shader::AddCBuffer(const std::filesystem::path & path)
	{
		std::ifstream file(path);
		auto shaderType = GetShaderType(path.filename().string());
		if (file.is_open())
		{
			std::string tokken;
			std::string CBName;
			while (!file.eof())
			{
				file >> tokken;
				if (tokken == "cbuffer")
				{
					file >> CBName;
					file >> tokken;
					file >> tokken;

					auto startReg = tokken.find('b');
					auto endReg = tokken.find(')');
					std::string v;
					for (size_t i = startReg + 1; i < endReg; ++i)
						v += tokken[i];

					int regNum = std::atoi(v.c_str());
					CBuffer::Type cbtype = GetCBType(CBName);
					if (cbtype != CBuffer::Type::None)
						CreateCBuffer(regNum, shaderType, cbtype);
				}
			}
		}
		file.close();
	}

	Shader::Shader(const std::string & path)
		: Path(path)
	{
		std::filesystem::directory_iterator	dirIter(path);
		auto i = path.rfind('/');
		Name = path.substr(i);

		LOG_INFO("Create {0} Shader", Name) {
			for (auto& file : dirIter)
			{
				Type type = GetShaderType(file.path().filename().string());

				if (type == Type::None || TypeKey & type) continue;

				TypeKey |= type;

				AddCBuffer(file.path());
				ID3D10Blob* binary = CompileShader(file.path(), type);
				ASSERT(binary, "Shader::Create shader binary failed");

				CreateShader(binary, type);
				LOG_MISC("Compile Success : {0}", file.path().filename().string());
			}
		}
	}

	void AddElements(const std::string& dataFormat, char* sementicName, uint32_t offset, std::vector<D3D11_INPUT_ELEMENT_DESC>& elementDescs)
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = sementicName;
		desc.SemanticIndex = 0;
		desc.Format = GetDxDataFormat(dataFormat);
		desc.InputSlot = 0;
		desc.AlignedByteOffset = offset;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;

		elementDescs.emplace_back(desc);
	}

	void Shader::SetLayout(const std::filesystem::path & path, ID3D10Blob* binary)
	{
		std::ifstream file(path);
		if (!file.is_open())
			return;

		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDescs;

		uint32_t elementCount = 0;
		uint32_t offset = 0;

		std::string tokken;

		while (!file.eof())
		{
			file >> tokken;
			if (tokken != "Input") continue;

			std::string dataFormat;
			file >> tokken;				// find "{"
			file >> dataFormat;
			while (dataFormat != "};")    // until find "};"
			{
				char* sementicName = new char[20];
				file >> tokken;			// varable name
				file >> tokken;			// :
				file >> sementicName;   // semtic name + ;
				sementicName[strlen(sementicName) - 1] = 0;

				if (!strcmp(sementicName, "TEXCOORD0"))
				{
					CreateSampler();
					sementicName[strlen(sementicName) - 1] = 0;
				}

				AddElements(dataFormat, sementicName, offset, elementDescs);
				offset += GetDxDataSize(dataFormat);
				file >> dataFormat;
			}							// example : float4 position : POSITION;
			break;
		}
		file.close();

		InputLayout.Stride = offset;
		auto ret = Dx11Core::Get().Device->CreateInputLayout(elementDescs.data(), (UINT)elementDescs.size(),
			binary->GetBufferPointer(), binary->GetBufferSize(), &InputLayout.Layout);

	}

	void Shader::CreateSampler()
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		Dx11Core::Get().Device->CreateSamplerState(&samplerDesc, &SamplerState);
	}

	void Shader::CreateShader(ID3D10Blob* binary, Type type)
	{
		switch (type)
		{
		case Shader::VertexShader:
		{
			ID3D11VertexShader* shader;
			Dx11Core::Get().Device->CreateVertexShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			ASSERT(shader, "Shader::Create Shader failed");
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::HullShader:
		{
			ID3D11HullShader* shader;
			Dx11Core::Get().Device->CreateHullShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			ASSERT(shader, "Shader::Create Shader failed");
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::DomainShader:
		{
			ID3D11DomainShader* shader;
			Dx11Core::Get().Device->CreateDomainShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			ASSERT(shader, "Shader::Create Shader failed");
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::GeometryShader:
		{
			ID3D11GeometryShader* shader;
			Dx11Core::Get().Device->CreateGeometryShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			ASSERT(shader, "Shader::Create Shader failed");
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::PixelShader:
		{
			ID3D11PixelShader* shader;
			Dx11Core::Get().Device->CreatePixelShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			ASSERT(shader, "Shader::Create Shader failed");
			Shaders.emplace(type, shader);
		}
		break;
		}
		binary->Release();
	}

	ID3D10Blob* Shader::CompileShader(const std::filesystem::path & path, Type type)
	{
		HRESULT result;
		ID3D10Blob* msg;
		ID3D10Blob* binary;

		std::string compilerName;

		switch (type)
		{
		case Shader::VertexShader: compilerName = "vs"; break;
		case Shader::HullShader: compilerName = "hs"; break;
		case Shader::DomainShader: compilerName = "ds"; break;
		case Shader::GeometryShader: compilerName = "gs"; break;
		case Shader::PixelShader: compilerName = "ps"; break;
		}
		compilerName += "_5_0";

		//Complie and Create
		result = D3DCompileFromFile(
			path.wstring().data(),
			NULL,
			NULL,
			"main",
			compilerName.data(),
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			&binary,
			&msg
		);
		if (result != S_OK)
			Dx11Core::ErrorMessage(msg);

		if (type == Shader::VertexShader)
			SetLayout(path, binary);

		return binary;
	}

	BufferBuilder Shader::CreateCompotibleBuffer()
	{
		BufferBuilder builder(InputLayout);
		return builder;
	}

	void Shader::Bind() const
	{
		Dx11Core::Get().Context->IASetInputLayout(InputLayout.Layout);

		for (auto&[type, shader] : Shaders)
		{
			switch (type)
			{
			case Type::VertexShader: Dx11Core::Get().Context->VSSetShader(std::get<ID3D11VertexShader*>(shader), NULL, 0); break;
			case Type::HullShader: Dx11Core::Get().Context->HSSetShader(std::get<ID3D11HullShader*>(shader), NULL, 0); break;
			case Type::DomainShader: Dx11Core::Get().Context->DSSetShader(std::get<ID3D11DomainShader*>(shader), NULL, 0); break;
			case Type::GeometryShader: Dx11Core::Get().Context->GSSetShader(std::get<ID3D11GeometryShader*>(shader), NULL, 0); break;
			case Type::PixelShader: Dx11Core::Get().Context->PSSetShader(std::get<ID3D11PixelShader*>(shader), NULL, 0); break;
			}
		}
		if (SamplerState)
		{
			Dx11Core::Get().Context->PSSetSamplers(0, 1, &SamplerState);
		}
	}

}
