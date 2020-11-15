#include "pch.h"

#include "ModuleCore.h"
#include "Renderer/Renderer.h"
#include "Renderer/PipelineController.h"
#include "Renderer/Shader.h"
#include "File/FbxLoader.h"
#include "Renderer/Texture.h"
#include "File/FileCommon.h"


namespace Engine {

	static bool isInited = false;

	bool ModuleCore::IsInitiated()
	{
		return isInited;
	}

	void ModuleCore::Init(const WindowProp & prop)
	{
		if (isInited) return;

		Renderer::Init(prop);
		Renderer::GetPipelineController().Bind(PipelineComponent::DepthStencil)
			.SetBlend(BlendOpt::Alpha)
			.SetDepthStencil(DepthStencilOpt::Enable)
			.SetRasterize(RasterlizerOpt::Solid);

		std::filesystem::directory_iterator ShaderFolder(File::GetCommonPath(File::Shader));
		for (auto& file : ShaderFolder)
		{
			if (file.is_directory())
				Renderer::CreateShader(file.path().string(), file.path().filename().string());
		}

		std::string fbxDir = File::GetCommonPath(File::FBX);
		std::filesystem::directory_iterator FbxFolder(fbxDir);
		for (auto& dir : FbxFolder)
		{
			if (dir.is_directory())
			{
				std::filesystem::directory_iterator CurFolder(dir.path());
				std::string specificDir = fbxDir + "\\" + dir.path().filename().string() + "\\";

				FBXLoader fbxLoader;
				if (!fbxLoader.Init(dir.path().stem().string()))
					continue;

				for (auto& file : CurFolder)
				{
					if (file.is_directory()) continue;

					fbxLoader.Extract(specificDir, file);
				}
			}
		}

		std::filesystem::recursive_directory_iterator TextureFolder(File::GetCommonPath(File::Texture));
		for (auto& dir : TextureFolder)
		{
			if (dir.is_regular_file())
			{
				TextureArchive::Add(dir.path().string(), dir.path().stem().string());
			}
		}
		isInited = true;
	}

	ModuleCore::~ModuleCore()
	{
	}

}
