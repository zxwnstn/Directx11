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
#include "ImGuiLayer.h"

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

		LOG_INFO("Initiated window size width : {0}, height : {1}", prop.Width, prop.Height);

		LOG_INFO("Init Renderer") {
			Renderer::Init(prop);
		}

		LOG_INFO("Compliing Shader") {
			std::filesystem::recursive_directory_iterator ShaderDir(File::GetCommonPath(File::Shader));
			for (auto dir : ShaderDir)
			{
				if (dir.is_regular_file())
				{
					std::string shaderName;
					std::filesystem::path edir = dir;
					while (1)
					{
						edir = edir.parent_path();
						std::string stem = edir.stem().string();
						if (stem == "Shader")
							break;
						shaderName = stem + shaderName;
					}

					auto abspath = std::filesystem::absolute(dir.path().parent_path());
					ShaderArchive::Add(abspath.string(), shaderName);
				}
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

		LOG_INFO("Load Texture") {
			std::filesystem::recursive_directory_iterator TextureFolder(File::GetCommonPath(File::Texture));
			std::vector<std::string> tempEnv;
			for (auto& dir : TextureFolder)
			{
				if (dir.is_regular_file())
				{
					if (dir.path().parent_path().stem() == "Environment")
					{
						if(!tempEnv.empty())
							continue;

						std::string path = dir.path().parent_path().string() + "\\";
						const char* compo[] = { "right.png", "left.png", "bottom.png", "top.png", "front.png", "back.png" };
						for (int i = 0; i < 6; ++i)
						{
							tempEnv.push_back(path + compo[i]);
						}
					}
					else
					{
						TextureArchive::Add(dir.path().string(), dir.path().stem().string());
					}
				}
			}
			TextureArchive::Add(tempEnv, "TempEnvironment", 1024, 1024, true);
			ShadowMap(1024, 1024, 6);
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

		LOG_INFO("Init ImGui") {
			ImGuiLayer::Init((int)prop.hWnd);
		}

		MeshArchive::GetStaticMesh("SkyBox")->Type = MeshType::SkyBox;

		isInited = true;
		LOG_CRITICAL("Initiation ModuleCore succesfully Complete {0}sec", Timestep::TotalElapse());
	}

	ModuleCore::~ModuleCore()
	{
	}

}
