#include "pch.h"
#include "FileCommon.h"

#include <filesystem>

namespace File {

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
		case File::FBXCache:
			return "../../Engine/assets/FbxCache/";
		case File::Shader:
			return "../../Engine/assets/Shader/";
		}
		return "";
	}

}

