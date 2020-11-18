#include "pch.h"

#include "Renderer.h"
#include "Dx11Core.h"
#include "PipelineController.h"
#include "Texture.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Model/Model.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Model/3D/Skeleton.h"

namespace Engine {

	static PipelineController* s_PLController = nullptr;
	static std::unordered_map<std::string, Shader> RendererShaders;
	static Environment* GlobalEnv = nullptr;

	void Renderer::Init(const struct WindowProp& prop)
	{
		Dx11Core::Get().Init(prop);
		s_PLController = new PipelineController;
		s_PLController->Init(prop);
		GlobalEnv = new Environment;
	}

	void Renderer::BeginScene(Camera & camera, Light& light)
	{
		Dx11Core::Get().ClearBackBuffer();
		Dx11Core::Get().Context->ClearDepthStencilView(s_PLController->m_DepthStencil.View, D3D11_CLEAR_DEPTH, 1.0f, 0);

		for (auto&[name, shader] : RendererShaders)
		{
			shader.SetParam<CBuffer::Camera>(camera);
			shader.SetParam<CBuffer::Environment>(*GlobalEnv);
			shader.SetParam<CBuffer::Light>(light);
		}
	}

	void Renderer::Enque(std::shared_ptr<Model3D> model)
	{
		RendererShaders[model->m_Shader].Bind();
		RendererShaders[model->m_Shader].SetParam<CBuffer::Bone>(model->m_Animation->MySkinnedTransforms);
		RendererShaders[model->m_Shader].SetParam<CBuffer::Transform>(model->m_Transform);
		RendererShaders[model->m_Shader].SetParam<CBuffer::Material>(*model->m_Material);

		Texture::MultipleBind(model->m_Textures);
		model->m_ModelBuffer->Bind();

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
	}

	void Renderer::Enque(std::shared_ptr<ModelBuffer> buffer, RenderingShader type)
	{
		RendererShaders[ToString(type)].Bind();
		buffer->Bind();
		Dx11Core::Get().Context->DrawIndexed(buffer->GetIndexCount(), 0, 0);
	}

	void Renderer::EndScene()
	{
		Dx11Core::Get().Present();
	}

	PipelineController& Renderer::GetPipelineController()
	{
		return *s_PLController;
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		Dx11Core::Get().Resize(width, height);
		s_PLController->Resize();
	}

	unsigned char * Renderer::GetBackBufferData()
	{
		return Dx11Core::Get().GetBackBufferData();
	}

	Shader& Renderer::GetShader(RenderingShader shader)
	{
		return RendererShaders[ToString(shader)];
	}

	Shader & Renderer::GetShader(const std::string & shader)
	{
		return RendererShaders[shader];
	}

	void Renderer::CreateShader(const std::string & path, const std::string & keyName)
	{
		auto find = RendererShaders.find(keyName);
		if (find != RendererShaders.end())
			return;

		RendererShaders.emplace(keyName, path);
	}

	void Renderer::DeleteShader(const std::string & keyName)
	{
		auto find = RendererShaders.find(keyName);
		if (find == RendererShaders.end())
			return;

		RendererShaders.erase(find);
	}

	Environment * Renderer::GetGlobalEnv()
	{
		return GlobalEnv;
	}

	std::string ToString(RenderingShader type)
	{
		switch (type)
		{
		case RenderingShader::Color: return "Color";
		case RenderingShader::Texture: return "Texture";
		case RenderingShader::Skinned: return "Skinned";
		case RenderingShader::Lighting: return "Lighting";
		}
		return "";
	}

}
