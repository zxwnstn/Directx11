#include "pch.h"
#include "FileCommon.h"
#include "Common/Timestep.h"
#include <filesystem>

namespace Engine::File {

	bool isExistFile(const std::string & filename)
	{
		return std::filesystem::is_regular_file(filename);
	}

	bool isExistDirectroy(const std::string & path)
	{
		return std::filesystem::is_directory(path);
	}

	void CreateDir(const std::string & path)
	{
		std::filesystem::create_directories(path);
	}

	void CreateFile_(const std::string & path)
	{
		std::ofstream ofs(path);
		ofs.close();
	}


	void TryCreateDir(const std::string& path)
	{
		if (isExistFile(path) || isExistDirectroy(path))
			return;
		CreateDir(path);
	}

	void TryCreateFile(const std::string& file)
	{
		if (isExistFile(file) || isExistDirectroy(file))
			return;

		size_t point = file.rfind('/');
		auto dir = file.substr(0, point);
		TryCreateDir(dir);
		CreateFile_(file);
	}

	std::string GetCommonPath(CommonPathType pathType)
	{
		switch (pathType)
		{
		case File::Assets:
			return "../../Engine/assets/";
		case File::Texture:
			return "../../Engine/assets/Texture/";
		case File::FBX:
			return "../../Engine/assets/Fbx/";
		case File::Obj:
			return "../../Engine/assets/Obj/";
		case File::FBXCache:
			return "../../Engine/assets/FbxCache/";
		case File::Shader:
			return "../../Engine/assets/Shader/";
		case File::Log:
			return "../../Log/" + Time::Year_Mon_Day();
		}
		return "";
	}

}

