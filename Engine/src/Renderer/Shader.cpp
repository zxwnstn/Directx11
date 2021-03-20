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
		case CBuffer::Type::CubeCamera: return sizeof(CBuffer::CubeCamera);
		case CBuffer::Type::Transform: return sizeof(CBuffer::Transform);
		case CBuffer::Type::Light: return sizeof(CBuffer::Light);
		case CBuffer::Type::LightPos: return sizeof(CBuffer::LightPos);
		case CBuffer::Type::LightCam: return sizeof(CBuffer::LightCam);
		case CBuffer::Type::LightColor: return sizeof(CBuffer::LightColor);
		case CBuffer::Type::Cascaded: return sizeof(CBuffer::Cascaded);
		case CBuffer::Type::CascadedViewProj: return sizeof(CBuffer::CascadedViewProj);
		case CBuffer::Type::Bone: return sizeof(CBuffer::Bone);
		case CBuffer::Type::Environment: return sizeof(CBuffer::Environment);
		case CBuffer::Type::Material: return sizeof(CBuffer::Material);
		case CBuffer::Type::Materials: return sizeof(CBuffer::Materials);
		case CBuffer::Type::TextureInform: return sizeof(CBuffer::TextureInform);
		case CBuffer::Type::TFactor: return sizeof(CBuffer::TFactor);
		case CBuffer::Type::DispatchInfo: return sizeof(CBuffer::DispatchInfo);
		case CBuffer::Type::ToneMapFactor: return sizeof(CBuffer::ToneMapFactor);
		case CBuffer::Type::Gamma: return sizeof(CBuffer::Gamma);
		case CBuffer::Type::SkyBoxInfo: return sizeof(CBuffer::SkyBoxInfo);
		}
		return 0;
	}

	CBuffer::Type GetCBType(const std::string& name)
	{
		if (name == "Camera") return CBuffer::Type::Camera;
		if (name == "CubeCamera") return CBuffer::Type::CubeCamera;
		if (name == "Light") return CBuffer::Type::Light;
		if (name == "LightPos") return CBuffer::Type::LightPos;
		if (name == "LightCam") return CBuffer::Type::LightCam;
		if (name == "LightColor") return CBuffer::Type::LightColor;
		if (name == "CascadedViewProj") return CBuffer::Type::CascadedViewProj;
		if (name == "Cascaded") return CBuffer::Type::Cascaded;
		if (name == "Transform") return CBuffer::Type::Transform;
		if (name == "Bone") return CBuffer::Type::Bone;
		if (name == "Environment") return CBuffer::Type::Environment;
		if (name == "Material") return CBuffer::Type::Material;
		if (name == "Materials") return CBuffer::Type::Materials;
		if (name == "TextureInform") return CBuffer::Type::TextureInform;
		if (name == "TFactor") return CBuffer::Type::TFactor;
		if (name == "DispatchInfo") return CBuffer::Type::DispatchInfo;
		if (name == "ToneMapFactor") return CBuffer::Type::ToneMapFactor;
		if (name == "Gamma") return CBuffer::Type::Gamma;
		if (name == "SkyBoxInfo") return CBuffer::Type::SkyBoxInfo;
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
		if (filename == "Compute.hlsl") return Shader::Type::ComputeShader;

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

	Shader::Shader(const std::string & path, const std::string& name)
		: Path(path), Name(name)
	{
		std::filesystem::directory_iterator	dirIter(path);

		LOG_TRACE("Shader::Create {0} Shader", Name) {
			for (auto& file : dirIter)
			{
				Type type = GetShaderType(file.path().filename().string());

				if (type == Type::None || TypeKey & type) continue;

				TypeKey |= type;

				AddCBuffer(file.path());
				ID3D10Blob* binary = CompileShader(file.path(), type);
				if (!binary)
				{
					LOG_CRITICAL("Shader::Create shader binary failed");
					Created = false;
					return;
				}

				CreateShader(binary, type);
				
				LOG_MISC("Compile Success : {0}", file.path().filename().string());
			}
		}
	}

	void Shader::Release()
	{

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

				AddElements(dataFormat, sementicName, offset, elementDescs);
				offset += GetDxDataSize(dataFormat);
				file >> dataFormat;
			}							// example : float4 position : POSITION;
			break;
		}
		file.close();

		Layout.Stride = offset;
		auto ret = Dx11Core::Get().Device->CreateInputLayout(elementDescs.data(), (UINT)elementDescs.size(),
			binary->GetBufferPointer(), binary->GetBufferSize(), &Layout.D11Layout);
	}

	void Shader::CreateSampler(const std::filesystem::path& path)
	{
		std::ifstream file(path);

		std::string tokken;
		while (!file.eof())
		{
			file >> tokken;
			if (tokken == "SV_TARGET") 
				break;
			if (tokken == "SamplerState" || tokken == "SamplerComparisonState")
			{
				file >> tokken;
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

				if (tokken == "SampleTypeClamp")
				{
					samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
					samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				}
				if (tokken == "SampleTypePCF")
				{
					samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
					samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
					samplerDesc.BorderColor[0] = 1.0;
					samplerDesc.BorderColor[1] = 1.0;
					samplerDesc.BorderColor[2] = 1.0;
					samplerDesc.BorderColor[3] = 1.0;
				}

				Dx11Core::Get().Device->CreateSamplerState(&samplerDesc, &SamplerState[SamplerNumber]);
				++SamplerNumber;
			}
		}
		file.close();
	}
	

	void Shader::CreateShader(ID3D10Blob* binary, Type type)
	{
		switch (type)
		{
		case Shader::VertexShader:
		{
			ID3D11VertexShader* shader = nullptr;
			Dx11Core::Get().Device->CreateVertexShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::HullShader:
		{
			ID3D11HullShader* shader = nullptr;
			Dx11Core::Get().Device->CreateHullShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::DomainShader:
		{
			ID3D11DomainShader* shader = nullptr;
			Dx11Core::Get().Device->CreateDomainShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::GeometryShader:
		{
			ID3D11GeometryShader* shader = nullptr;
			if (SOLayout.empty())
			{
				Dx11Core::Get().Device->CreateGeometryShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			}
			else
			{
				uint32_t stride = 0;
				for (auto& elem : SOLayout)
					stride += elem.ComponentCount;
				stride *= 4;

				Dx11Core::Get().Device->CreateGeometryShaderWithStreamOutput(
					binary->GetBufferPointer(),binary->GetBufferSize(), 
					SOLayout.data(), (UINT)SOLayout.size(), &stride, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, &shader);
			}
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::PixelShader:
		{
			ID3D11PixelShader* shader = nullptr;
			Dx11Core::Get().Device->CreatePixelShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		case Shader::ComputeShader:
		{
			ID3D11ComputeShader* shader = nullptr;
			Dx11Core::Get().Device->CreateComputeShader(binary->GetBufferPointer(), binary->GetBufferSize(), nullptr, &shader);
			if (!shader)
			{
				LOG_CRITICAL("Shader::Create Shader failed");
				Created = false;
				return;
			}
			Shaders.emplace(type, shader);
		}
		break;
		}
		
		binary->Release();
	}

	void Shader::CheckStreamOut(const std::filesystem::path & path)
	{
		std::ifstream file(path);
		std::string tokken;

		while (!file.eof())
		{
			file >> tokken;
			if (tokken == "StreamOutput")
			{
				file >> tokken;
				file >> tokken;
				while (tokken != "};")
				{
					D3D11_SO_DECLARATION_ENTRY so{ 0, };
					char* sementicName = new char[20];
					
					so.ComponentCount = GetDxDataSize(tokken) / 4;  // data type
					file >> tokken;									// var name
					file >> tokken;									// : 
					file >> sementicName;							// Sementic name
					sementicName[strlen(sementicName) - 1] = 0;
					so.SemanticName = sementicName;

					SOLayout.push_back(so);

					file >> tokken;
				}
				break;
			}
		}
		file.close();

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
		case Shader::ComputeShader: compilerName = "cs"; break;
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
		{
			Dx11Core::ErrorMessage(msg);
			Created = false;
			return nullptr;
		}

		if (type == Shader::VertexShader)
			SetLayout(path, binary);
		if (type == Shader::PixelShader)
			CreateSampler(path);
		if (type == Shader::GeometryShader)
			CheckStreamOut(path);

		return binary;
	}

	BufferBuilder Shader::CreateCompotibleBuffer()
	{
		BufferBuilder builder(MeshType::Skeletal);
		return builder;
	}

	void Shader::Bind() const
	{
		if (TypeKey == Type::ComputeShader)
		{
			Dx11Core::Get().Context->CSSetShader(std::get<ID3D11ComputeShader*>(Shaders.begin()->second), NULL, 0);
			for (uint32_t i = 0; i < SamplerNumber; ++i)
			{
				Dx11Core::Get().Context->CSSetSamplers(i, 1, &SamplerState[i]);
			}
			return;
		}

		Dx11Core::Get().Context->IASetInputLayout(Layout.D11Layout);
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
		for(uint32_t i = 0 ; i < SamplerNumber; ++i)
		{
			Dx11Core::Get().Context->PSSetSamplers(i, 1, &SamplerState[i]);
		}
	}

	void Shader::Unbind()
	{
		ID3D11VertexShader* vs = nullptr;
		ID3D11PixelShader* ps = nullptr;
		ID3D11GeometryShader* gs = nullptr;
		ID3D11HullShader* hs = nullptr;
		ID3D11DomainShader* ds = nullptr;
		ID3D11ComputeShader* cs = nullptr;
		ID3D11SamplerState* ss = nullptr;
		Dx11Core::Get().Context->VSSetShader(vs, NULL, 0);
		Dx11Core::Get().Context->HSSetShader(hs, NULL, 0);
		Dx11Core::Get().Context->DSSetShader(ds, NULL, 0);
		Dx11Core::Get().Context->PSSetShader(ps, NULL, 0);
		Dx11Core::Get().Context->GSSetShader(gs, NULL, 0);
		Dx11Core::Get().Context->CSSetShader(cs, NULL, 0);

		for (uint32_t i = 0; i < SamplerNumber; ++i)
		{
			Dx11Core::Get().Context->PSSetSamplers(i, 1, &ss);
		}
	}

	void Shader::Dipatch(uint32_t x, uint32_t y, uint32_t z)
	{
		if (TypeKey != Type::ComputeShader) return;
		Dx11Core::Get().Context->Dispatch(x, y, z);
	}

	bool Shader::Has(Type type)
	{
		return TypeKey & static_cast<uint32_t>(type);
	}

	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderArchive::s_Shaders;

	std::shared_ptr<Shader> ShaderArchive::Add(const std::string& path, const std::string& name)
	{
		if (!Has(name))
		{
			auto shader = new Shader(path, name);
			ASSERT(shader->Created, "Shader Create Failed");
			s_Shaders[name].reset(shader);
		}
		return s_Shaders[name];
	}

	std::shared_ptr<Shader> ShaderArchive::Get(const std::string& name)
	{
		if (!Has(name))
		{
			ASSERT(true, name + "Shader does not exist");
			return nullptr;
		}
		return s_Shaders[name];
	}

	bool ShaderArchive::Has(const std::string & name)
	{
		auto find = s_Shaders.find(name);
		return find != s_Shaders.end();
	}

	void ShaderArchive::RecomplieShader(const std::string & path)
	{
		std::string target;
		auto find = path.rfind("\\");
		std::string folder = path.substr(0, find);

		for (auto shader : s_Shaders)
		{
			if (shader.second->Path == folder)
			{
				target = shader.first;
			}
		}

		auto shader = new Shader(folder, target);
		if (!shader->Created)
		{
			LOG_CRITICAL("{0} Shader recompiling filed", target);
			return;
		}
		LOG_INFO("{0} Shader recompiling complete!", target);
		s_Shaders[target]->Release();
		s_Shaders[target].reset(shader);
	}

}
