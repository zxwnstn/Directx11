#pragma once

namespace File {

	bool isExistFile(const std::string& filename);
	bool isExistDirectroy(const std::string& filename);

	template<typename Func>
	void iterateDirectory(const std::string& directroy, Func& func)
	{

	}
}
