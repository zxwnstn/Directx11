#include "pch.h"

#include "Renderer.h"
#include "Dx11Core.h"
#include "PipelineController.h"
#include "Model/Texture.h"
#include "Common/Camera.h"

static PipelineController* s_PLController = nullptr;
static std::unordered_map<std::string, Shader> RendererShaders;

std::string ToString(DefaultShader type)
{
	switch (type)
	{
	case DefaultShader::Color: return "Color";
	case DefaultShader::Texture: return "Texture";
	case DefaultShader::Skinned: return "Skinned";
	}
	return "";
}


void Renderer::Init()
{
	Dx11Core::Get().Init();
	s_PLController = new PipelineController;
	s_PLController->Init();
}

void Renderer::BeginScene(Camera & camera)
{
	Dx11Core::Get().ClearBackBuffer();
	Dx11Core::Get().Context->ClearDepthStencilView(s_PLController->m_DepthStencil.View, D3D11_CLEAR_DEPTH, 1.0f, 0);

	for (auto&[name, shader] : RendererShaders)
	{
		shader.SetParam((CBuffer::Camera)camera);
	}

}

void Renderer::Enque(DefaultShader shader, const ModelBuffer & buffer)
{
	RendererShaders[ToString(shader)].Bind();
	buffer.Bind();

	Dx11Core::Get().Context->DrawIndexed(buffer.GetIndexCount(), 0, 0);
}

void Renderer::Enque(DefaultShader shader, const ModelBuffer & buffer, const Texture & texture)
{
	RendererShaders[ToString(shader)].Bind();
	buffer.Bind();
	texture.Bind();

	Dx11Core::Get().Context->DrawIndexed(buffer.GetIndexCount(), 0, 0);
}


void Renderer::EndScene()
{
	Dx11Core::Get().Present();
}

PipelineController& Renderer::GetPipelineController() 
{ 
	return *s_PLController; 
}

Shader& Renderer::GetDefaultShader(DefaultShader shader)
{
	return RendererShaders[ToString(shader)];
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

