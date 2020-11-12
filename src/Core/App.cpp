#include "pch.h"

#include "App.h"
#include "Renderer/Renderer.h"
#include "Renderer/PipelineController.h"
#include "Renderer/Shader.h"
#include "File/FbxLoader.h"

bool App::s_Running;

App::App(const WindowProp & prop)
{
	s_Running = true;
	Window::Init(prop);
	Renderer::Init();
	Renderer::GetPipelineController().Bind(PipelineComponent::DepthStencil)
		.SetBlend(BlendOpt::Alpha)
		.SetDepthStencil(DepthStencilOpt::Enable)
		.SetRasterize(RasterlizerOpt::Solid);

	std::filesystem::directory_iterator ShaderFolder("assets\\Shader");
	for (auto& file : ShaderFolder)
	{
		if (file.is_directory())
			Renderer::CreateShader(file.path().string(), file.path().filename().string());
	}

	std::string fbxDir = "assets\\Fbx";
	std::filesystem::directory_iterator FbxFolder(fbxDir);
	for (auto& dir : FbxFolder)
	{
		if (dir.is_directory())
		{
			std::filesystem::directory_iterator CurFolder(dir.path());
			std::string specificDir = fbxDir + "\\" + dir.path().filename().string() + "\\";
			
			FBXLoader fbxLoader(dir.path().filename().string());
			for (auto& file : CurFolder)
			{
				if (file.is_directory()) continue;
				fbxLoader.Extract(specificDir, file.path().filename().string());
			}
		}

	}
}

App::~App()
{
	for (auto&[name, layer] : Layers)
	{
		PopLayer(name);
	}
	Layers.clear();
}

int32_t App::Run()
{
	while (s_Running)
	{
		for (auto& layer : Layers)
		{
			layer.second->OnUpdate(1.0f / 60.0f);
		}
		Window::Update();
	}

	return 0;
}

void App::PushLayer(Layer * layer, const std::string & name)
{
	auto find = Layers.find(name);
	if (find != Layers.end())
		return;

	layer->OnAttach();
	Layers.emplace(name, layer);
}

void App::PopLayer(const std::string & name)
{
	auto find = Layers.find(name);
	if (find == Layers.end())
		return;

	find->second->OnDettach();
}
