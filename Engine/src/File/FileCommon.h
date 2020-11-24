#pragma once

namespace Engine::File {

	enum CommonPathType
	{
		Assets,
		Texture,
		FBX,
		Obj,
		FBXCache,
		Shader,
		Log
	};

	bool isExistFile(const std::string& filename);
	bool isExistDirectroy(const std::string& path);
	void CreateDir(const std::string& path);
	void CreateFile_(const std::string& path);
	void TryCreateDir(const std::string& path);
	void TryCreateFile(const std::string& file);

	std::string GetCommonPath(CommonPathType pathType);
}
