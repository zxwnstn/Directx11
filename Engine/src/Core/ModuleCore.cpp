#include "pch.h"

#include "ModuleCore.h"
#include "Renderer/Renderer.h"
#include "Renderer/PipelineController.h"
#include "Renderer/Shader.h"
#include "File/FbxLoader.h"
#include "Renderer/Texture.h"
#include "File/FileCommon.h"
#include "Common/Timestep.h"
#include "Common/Mesh.h"
#include "File/ObjLoader.h"

namespace Engine {

	static bool isInited = false;

	bool ModuleCore::IsInitiated()
	{
		return isInited;
	}

	void ModuleCore::Init(const WindowProp & prop)
	{
		if (isInited) return;

		Timestep::Init();
		Log::Init();
		MeshArchive::Init();

		ENABLE_ELAPSE
		LOG_CRITICAL("Hello Directx11 Engine! initiation ModuleCore start");

		LOG_INFO("Init Renderer") {
			Renderer::Init(prop);
			Renderer::GetPipelineController()
				.SetBlend(BlendOpt::Alpha)
				.SetDepthStencil(DepthStencilOpt::Enable)
				.SetRasterize(RasterlizerOpt::Solid);
		}

		LOG_INFO("Compliing Shader") {
			std::filesystem::directory_iterator ShaderFolder(File::GetCommonPath(File::Shader));
			for (auto& file : ShaderFolder)
			{
				if (file.is_directory())
					Renderer::CreateShader(file.path().string(), file.path().filename().string());
			}
		}

		LOG_INFO("Load Obj") {
			std::filesystem::directory_iterator ObjFolder(File::GetCommonPath(File::Obj));
			for (auto& dir : ObjFolder)
			{
				//file.is_regular_file() && file.path().extension().string() == ".obj"
				if (dir.is_directory())
				{
					std::filesystem::directory_iterator CurFolder(dir.path());
					for (auto& file : CurFolder)
					{
						if (file.is_regular_file() && file.path().extension().string() == ".obj")
						{
							ObjLoader objLoader(file.path());
							objLoader.Extract();
						}
					}
				}
			}
		}
		
		LOG_INFO("Load Fbx") {
			std::string fbxDir = File::GetCommonPath(File::FBX);
			std::filesystem::directory_iterator FbxFolder(fbxDir);
			for (auto& dir : FbxFolder)
			{
				if (dir.is_directory())
				{
					std::filesystem::directory_iterator CurFolder(dir.path());
					std::string specificDir = fbxDir + dir.path().filename().string() + "/";

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
		}
		

		LOG_INFO("Load Texture") {
			std::filesystem::recursive_directory_iterator TextureFolder(File::GetCommonPath(File::Texture));
			for (auto& dir : TextureFolder)
			{
				if (dir.is_regular_file())
				{
					TextureArchive::Add(dir.path().string(), dir.path().stem().string());
				}
			}
		}
		LOG_INFO("Prepare 2D rendering") {
			Renderer::prep2D();
			Renderer::prepSkyCube();
		}

		isInited = true;
		LOG_CRITICAL("Initiation ModuleCore succesfully Complete {0}sec", Timestep::TotalElapse());

	}

	ModuleCore::~ModuleCore()
	{
	}

}
