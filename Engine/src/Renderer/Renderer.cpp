#include "pch.h"

#include "Renderer.h"
#include "Dx11Core.h"
#include "PipelineController.h"
#include "Texture.h"
#include "Common/Camera.h"
#include "Model/Model.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Model/3D/Skeleton.h"

namespace Engine {

	static PipelineController* s_PLController = nullptr;
	static std::unordered_map<std::string, Shader> RendererShaders;

	void Renderer::Init(const struct WindowProp& prop)
	{
		Dx11Core::Get().Init(prop);
		s_PLController = new PipelineController;
		s_PLController->Init(prop);
	}

	void Renderer::BeginScene(Camera & camera)
	{
		Dx11Core::Get().ClearBackBuffer();
		Dx11Core::Get().Context->ClearDepthStencilView(s_PLController->m_DepthStencil.View, D3D11_CLEAR_DEPTH, 1.0f, 0);

		for (auto&[name, shader] : RendererShaders)
		{
			shader.SetCameraParam((CBuffer::Camera)camera);
		}

	}

	void Renderer::Enque(RenderingShader shader, const ModelBuffer & buffer)
	{
		RendererShaders[ToString(shader)].Bind();
		buffer.Bind();

		Dx11Core::Get().Context->DrawIndexed(buffer.GetIndexCount(), 0, 0);
	}

	void Renderer::Enque(RenderingShader shader, const ModelBuffer & buffer, const Texture & texture)
	{
		RendererShaders[ToString(shader)].Bind();
		buffer.Bind();
		texture.Bind(0);

		Dx11Core::Get().Context->DrawIndexed(buffer.GetIndexCount(), 0, 0);
	}

	void Renderer::Enque(std::shared_ptr<Model3D> model)
	{
		RendererShaders[model->m_Shader].Bind();
		RendererShaders[model->m_Shader].SetBoneParam(model->m_Animation->MySkinnedTransforms, (uint32_t)model->m_Skeleton->Joints.size());
		RendererShaders[model->m_Shader].SetTransformParam(model->m_Transform);

		for (int i = 0; i < model->m_Textures.size(); ++i)
			model->m_Textures[i]->Bind(i);
		model->m_ModelBuffer->Bind();

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
	}

	void Renderer::EndScene()
	{
		Dx11Core::Get().Present();
	}

	PipelineController& Renderer::GetPipelineController()
	{
		return *s_PLController;
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

	std::string ToString(RenderingShader type)
	{
		switch (type)
		{
		case RenderingShader::Color: return "Color";
		case RenderingShader::Texture: return "Texture";
		case RenderingShader::Skinned: return "Skinned";
		}
		return "";
	}

}
