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
			return "assets/";
		case File::Texture:
			return "assets/Texture/";
		case File::FBX:
			return "assets/Fbx/";
		case File::FBXCache:
			return "assets/FbxCache/";
		case File::Shader:
			return "assets/Shader/";
		}
	}

}

