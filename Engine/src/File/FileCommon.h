#pragma once

namespace Engine::File {

	enum CommonPathType
	{
		Assets,
		Texture,
		FBX,
		FBXCache,
		Shader,
	};

	bool isExistFile(const std::string& filename);
	bool isExistDirectroy(const std::string& path);
	void CreateDir(const std::string& path);

	std::string GetCommonPath(CommonPathType pathType);

}
